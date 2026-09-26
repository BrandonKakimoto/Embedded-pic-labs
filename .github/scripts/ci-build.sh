#!/usr/bin/env bash
# Compile every *.X project the way the grader does, with -Wall, and turn
# compiler output into GitHub annotations plus a summary table.
# Exit non-zero if any project fails to build or produces no hex.
set -u
CPU=18F46K22
SUMMARY="${GITHUB_STEP_SUMMARY:-/dev/null}"
fail=0
details=""

{
  echo "## Compile results"
  echo
  echo "| Project | Result | Warnings (your files) | Program flash | Data RAM | Config bits | firmware.hex |"
  echo "|---|---|---:|---|---|---|---|"
} >> "$SUMMARY"

PROVIDED=$( [ -f .github/provided.sha256 ] && awk '{print $2}' .github/provided.sha256 || true )
is_provided() { echo "$PROVIDED" | grep -qxF "$1"; }

annotate() {   # $1 = project dir, stdin = compiler log -> annotations, skipping provided files, one per file:line
  # clang-style:  file:line:col: warning: msg
  # xc8 backend:  file:line:: warning: (520) msg
  sed -nE "s#^([A-Za-z0-9_./-]+):([0-9]+):[0-9]*:? *(warning|error): *(.*)#\3\t$1/\1\t\2\t\4#p" \
    | sed -E 's#\t([^/\t]+)/\./#\t\1/#' \
    | awk -F'\t' '!seen[$1 FS $2 FS $3]++' \
    | while IFS=$'\t' read -r level file line msg; do
        is_provided "$file" && continue
        echo "::$level file=$file,line=$line::$msg"
      done
}
count_own_warnings() {  # warnings not in provided files, one per file:line
  sed -nE "s#^([A-Za-z0-9_./-]+):([0-9]+):[0-9]*:? *warning: *(.*)#$1/\1\t\2#p" \
    | sed -E 's#^([^/\t]+)/\./#\1/#' | sort -u \
    | while IFS=$'\t' read -r file line; do is_provided "$file" || echo x; done | wc -l | tr -d ' '
}

for proj in *.X; do
  [ -d "$proj" ] || continue
  log=$(mktemp)
  echo "::group::Building $proj"
  (
    cd "$proj" || exit 1
    rm -f firmware.* *.p1 *.d *.o *.obj *.cmf *.hxl *.sdb *.sym *.rlf *.lst 2>/dev/null
    if ls *.c >/dev/null 2>&1; then
      SRC="*.c"; ls *.S >/dev/null 2>&1 && SRC="*.c *.S"
      xc8-cc -mcpu=$CPU -Wall -o firmware.elf $SRC
    elif ls *.S >/dev/null 2>&1; then
      pic-as -mcpu=$CPU -o firmware.hex *.S
    else
      echo "error: no .c or .S source files in $proj"; exit 1
    fi
  ) > "$log" 2>&1
  rc=$?
  cat "$log"
  ann=$(annotate "$proj" < "$log")
  echo "$ann"
  details="$details"$(printf '\n<details><summary><code>%s</code>: %s message(s) in your files</summary>\n\n```\n%s\n```\n</details>\n' "$proj" "$(echo "$ann" | grep -c . || true)" "$(echo "$ann" | sed -E 's/^::(warning|error) file=([^,]+),line=([0-9]+)::/\1: \2:\3: /' | grep . || echo none)")

  warnings=$(count_own_warnings "$proj" < "$log")
  allw=$(grep -cE ': *warning:' "$log" || true)
  prog=$(grep -E 'Program space' "$log" | grep -oE '\( *[0-9.]+%\)' | tr -d '() ' || true)
  data=$(grep -E 'Data space' "$log" | grep -oE '\( *[0-9.]+%\)' | tr -d '() ' || true)
  cfg=$(grep -E 'Configuration bits' "$log" | sed -E 's/.*used +([0-9A-Fa-f]+)h.*of +([0-9A-Fa-f]+)h.*/\1h of \2h/' || true)
  hex="missing"; [ -f "$proj/firmware.hex" ] && hex="$(wc -c < "$proj/firmware.hex" | tr -d ' ') bytes"

  if echo "$cfg" | grep -qE '^0+h'; then
    echo "::warning file=$proj::$proj sets NO configuration bits (no #pragma config / CONFIG). The chip will run with default fuses - almost never what you want."
  fi

  if [ $rc -eq 0 ] && [ -f "$proj/firmware.hex" ]; then
    result="PASS"
    echo "PASS: $proj compiled ($warnings warning(s) in your files, $allw total)"
  else
    result="**FAIL**"; fail=1
    first=$(grep -m1 -E ': *error:|^error:|fatal' "$log" | cut -c1-200)
    echo "::error file=$proj::$proj failed to build: ${first:-see log}"
  fi
  echo "| \`$proj\` | $result | $warnings | ${prog:-–} | ${data:-–} | ${cfg:-–} | $hex |" >> "$SUMMARY"
  echo "::endgroup::"
  rm -f "$log"
done

{
  echo
  echo "### Compiler messages in your files (full list; the Annotations panel shows at most 10 per job)"
  echo "$details"
  echo
  echo "Warnings are listed under **Annotations** above. A PASS means the project assembles/compiles and links; it says nothing about behavior on the board."
} >> "$SUMMARY"
exit $fail
