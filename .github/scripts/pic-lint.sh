#!/usr/bin/env bash
# Heuristic checks for classic PIC18 / XC8 mistakes, plus lab-specific checks
# from .github/lab-checks.txt. Warnings only; never fails the build. Skips
# files listed as provided in .github/provided.sha256 and lower-case .s.
# Writes a machine-readable line per project to $LINT_TSV (if set):
#   proj<TAB>findings<TAB>todos<TAB>rule-ids;...
set -u
SUMMARY="${GITHUB_STEP_SUMMARY:-/dev/null}"
LINT_TSV="${LINT_TSV:-/dev/null}"
HERE="$(cd "$(dirname "$0")" && pwd)"
LABCHECKS="$HERE/../lab-checks.txt"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
provided=$( [ -f .github/provided.sha256 ] && awk '{print $2}' .github/provided.sha256 || true )
total=0; todos_all=0
{ echo; echo "## Lint (heuristics, warnings only)"; echo; echo "| Project | Generic findings | Lab-specific | TODOs left |"; echo "|---|---:|---:|---:|"; } >> "$SUMMARY"

strip() { perl -0777 -pe 's{/\*.*?\*/}{ $& =~ tr/\n//cdr }gse; s{//[^\n]*}{}g; s{"(?:\\.|[^"\\])*"}{""}g' "$1"; }
strip_asm() { sed -E 's/;.*$//' "$1"; }
hits=""
warn() { # id file line message
  echo "::warning file=$2,line=$3::[$1] $4"; echo "  $2:$3: [$1] $4"; hits="$hits$1;"
}

for proj in *.X; do
  [ -d "$proj" ] || continue
  [ "$proj" = "Lab1.X" ] && continue
  findings=0; labfind=0; todos=0; hits=""
  cfiles=(); sfiles=(); first=""
  for f in "$proj"/*.c "$proj"/*.h "$proj"/*.S; do
    [ -f "$f" ] || continue
    echo "$provided" | grep -qxF "$f" && continue
    [ -z "$first" ] && first="$f"
    case "$f" in *.S) sfiles+=("$f");; *) cfiles+=("$f");; esac
  done
  [ -z "$first" ] && continue
  own="$tmp/own_$proj.txt"; : > "$own"
  for f in ${cfiles[@]+"${cfiles[@]}"}; do strip "$f" >> "$own"; done
  for f in ${sfiles[@]+"${sfiles[@]}"}; do strip_asm "$f" >> "$own"; done
  allsrc=$(cat "$proj"/*.c "$proj"/*.h "$proj"/*.S 2>/dev/null | perl -0777 -pe 's{/\*.*?\*/}{}gs; s{//[^\n]*}{}g; s{;[^\n]*}{}g')
  has_xtal=$(echo "$allsrc" | grep -cE '\b_XTAL_FREQ\b' || true)
  has_ansel=$(echo "$allsrc" | grep -cE '\bANSEL[A-E]' || true)
  has_tris=$(echo "$allsrc" | grep -cE '\bTRIS[A-E]' || true)
  has_gie=$(echo "$allsrc" | grep -cE '\bGIE\b|INTCON\s*[|=]' || true)
  has_flagclear=$(echo "$allsrc" | grep -cE '\b[A-Z0-9]+IF\s*=\s*0|bcf\s+[A-Z0-9]+,\s*[A-Z0-9]*IF' || true)

  # ---------- generic C ----------
  for f in ${cfiles[@]+"${cfiles[@]}"}; do
    s="$tmp/$(echo "$f" | tr '/' '_')"; strip "$f" > "$s"
    todos=$((todos + $(grep -c 'TODO' "$f" || true)))
    while IFS=: read -r ln _; do [ -n "$ln" ] && { warn C1 "$f" "$ln" "Writing to PORTx does a read-modify-write on the PINS; drive outputs through LATx and read inputs from PORTx."; findings=$((findings+1)); }; done < <(grep -nE '\bPORT[A-E](bits\.R[A-E][0-7])?\s*(=[^=]|\|=|&=|\^=|\+\+|--)' "$s")
    if grep -qE '__delay_(ms|us)\s*\(' "$s" && [ "$has_xtal" -eq 0 ]; then
      warn C2 "$f" "$(grep -nE '__delay_(ms|us)\s*\(' "$s" | head -1 | cut -d: -f1)" "__delay_ms/us used but _XTAL_FREQ is not defined anywhere in $proj; the delay length is undefined."; findings=$((findings+1)); fi
    if grep -qE '\bmain\s*\(' "$s" && ! grep -qE 'while\s*\(\s*(1|true)\s*\)|for\s*\(\s*;\s*;\s*\)' "$s"; then
      warn C3 "$f" "$(grep -nE '\bmain\s*\(' "$s" | head -1 | cut -d: -f1)" "main() has no while(1)/for(;;) loop. On a microcontroller main() must never return."; findings=$((findings+1)); fi
    while IFS=: read -r ln _; do [ -n "$ln" ] && { warn C4 "$f" "$ln" "Assignment (=) inside a condition; did you mean == ?"; findings=$((findings+1)); }; done < <(grep -nE '\b(if|while)\s*\(\s*[A-Za-z_][A-Za-z0-9_.]*\s*=\s*[^=]' "$s")
    if grep -qE '__interrupt' "$s"; then
      ln=$(grep -nE '__interrupt' "$s" | head -1 | cut -d: -f1)
      [ "$has_gie" -eq 0 ] && { warn C5 "$f" "$ln" "An interrupt function exists but GIE is never set; it will never run."; findings=$((findings+1)); }
      [ "$has_flagclear" -eq 0 ] && { warn C6 "$f" "$ln" "No interrupt flag (xxIF = 0) is cleared anywhere; the ISR will re-enter forever."; findings=$((findings+1)); }
    fi
    if grep -qE '\bPORT[A-E](bits\.R[A-E][0-7])?\b' "$s" && [ "$has_ansel" -eq 0 ]; then
      warn C7 "$f" "$(grep -nE '\bPORT[A-E]' "$s" | head -1 | cut -d: -f1)" "PORTx is read but no ANSELx is configured in $proj; analog-default pins always read 0."; findings=$((findings+1)); fi
  done
  if [ ${#cfiles[@]} -gt 0 ] && [ "$has_tris" -eq 0 ] && grep -qE '\b(LAT|PORT)[A-E]' <<<"$allsrc"; then
    warn C8 "${cfiles[0]}" 1 "No TRISx assignment in $proj; pins default to inputs, so outputs will not drive."; findings=$((findings+1)); fi
  # ---------- generic asm ----------
  for f in ${sfiles[@]+"${sfiles[@]}"}; do
    s="$tmp/$(echo "$f" | tr '/' '_')"; strip_asm "$f" > "$s"
    todos=$((todos + $(grep -c 'TODO' "$f" || true)))
    while IFS=: read -r ln _; do [ -n "$ln" ] && { warn A1 "$f" "$ln" "ANSELx (0xF38-0xF3C) is BELOW the access bank: this write silently lands in ordinary RAM and the pins stay analog. Use banksel ANSELx + BANKMASK(ANSELx),b then movlb 0."; findings=$((findings+1)); }; done < <(grep -nE '\b(clrf|setf|movwf|bcf|bsf|movff)\s+(BANKMASK\()?ANSEL[A-E]\)?\s*(,\s*[0-7]\s*)?(,\s*a\s*)?$' "$s" | grep -vE ',\s*b\s*$')
    grep -qE '#include\s*<xc\.inc>' "$f" || { warn A2 "$f" 1 "No #include <xc.inc>; register names and CONFIG will not resolve."; findings=$((findings+1)); }
    while IFS=: read -r ln _; do [ -n "$ln" ] && { warn A3 "$f" "$ln" "retfie without ',1' does not restore W/STATUS/BSR from the shadow registers."; findings=$((findings+1)); }; done < <(grep -nE '^\s*retfie\s*$' "$s")
  done
  if grep -rqsE '\-p(resetVec|intVec)' "$proj" 2>/dev/null; then warn A4 "$proj" 1 "Obsolete -presetVec/-pintVec linker option found; the vectors are absolute psects now and these flags make the build fail."; findings=$((findings+1)); fi

  # ---------- lab-specific ----------
  if [ -f "$LABCHECKS" ]; then
    while IFS=$'\t' read -r lab id kind pattern msg; do
      [ "$lab" = "$proj" ] || continue
      case "$kind" in
        missing) grep -qE -- "$pattern" "$own" || { warn "$id" "$first" 1 "$msg"; labfind=$((labfind+1)); } ;;
        found)   for f in ${cfiles[@]+"${cfiles[@]}"} ${sfiles[@]+"${sfiles[@]}"}; do
                   s="$tmp/$(echo "$f" | tr '/' '_')"
                   if tr '\n' ' ' < "$s" | grep -qE -- "$pattern"; then
                     ln=$(grep -nE -- "${pattern%%\\s*}" "$s" | head -1 | cut -d: -f1); warn "$id" "$f" "${ln:-1}" "$msg"; labfind=$((labfind+1)); fi
                 done ;;
      esac
    done < <(grep -vE '^\s*(#|$)' "$LABCHECKS")
  fi

  total=$((total+findings+labfind)); todos_all=$((todos_all+todos))
  echo "| \`$proj\` | $findings | $labfind | $todos |" >> "$SUMMARY"
  [ "$todos" -gt 0 ] && echo "::notice file=$proj::$proj still has $todos TODO marker(s)."
  printf '%s\t%s\t%s\t%s\n' "$proj" "$((findings+labfind))" "$todos" "$hits" >> "$LINT_TSV"
done
{ echo; echo "$total finding(s), $todos_all TODO marker(s) remaining. Lint never fails the build; the [id] on each warning is the rule, lab-specific ones start with L."; } >> "$SUMMARY"
echo "lint: $total finding(s), $todos_all TODO(s)"
exit 0
