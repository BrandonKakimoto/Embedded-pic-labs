#!/usr/bin/env bash
# Heuristic checks for classic PIC18 / XC8 mistakes. Warnings only; never
# fails the build. Skips files listed as provided in .github/provided.sha256
# (you did not write those) and lower-case .s (compiler-generated).
set -u
SUMMARY="${GITHUB_STEP_SUMMARY:-/dev/null}"
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT
provided=$( [ -f .github/provided.sha256 ] && awk '{print $2}' .github/provided.sha256 || true )
total=0; todos_all=0
{ echo; echo "## Lint (heuristics, warnings only)"; echo; echo "| Project | Findings | TODOs left |"; echo "|---|---:|---:|"; } >> "$SUMMARY"

strip() {  # remove C comments and string literals, keep line numbers
  perl -0777 -pe 's{/\*.*?\*/}{ $& =~ tr/\n//cdr }gse; s{//[^\n]*}{}g; s{"(?:\\.|[^"\\])*"}{""}g' "$1"
}
strip_asm() { sed -E 's/;.*$//' "$1"; }

warn() { # file line message
  echo "::warning file=$1,line=$2::$3"; echo "  $1:$2: $3"
}

for proj in *.X; do
  [ -d "$proj" ] || continue
  [ "$proj" = "Lab1.X" ] && continue   # complete worked example, nothing to lint
  findings=0; todos=0
  cfiles=(); sfiles=()
  for f in "$proj"/*.c "$proj"/*.h "$proj"/*.S; do
    [ -f "$f" ] || continue
    echo "$provided" | grep -qxF "$f" && continue
    case "$f" in *.S) sfiles+=("$f");; *) cfiles+=("$f");; esac
  done
  # project-wide facts (all files, including provided)
  allsrc=$(cat "$proj"/*.c "$proj"/*.h "$proj"/*.S 2>/dev/null | perl -0777 -pe 's{/\*.*?\*/}{}gs; s{//[^\n]*}{}g; s{;[^\n]*}{}g')
  has_xtal=$(echo "$allsrc" | grep -cE '\b_XTAL_FREQ\b' || true)
  has_ansel=$(echo "$allsrc" | grep -cE '\bANSEL[A-E]' || true)
  has_tris=$(echo "$allsrc" | grep -cE '\bTRIS[A-E]' || true)
  has_gie=$(echo "$allsrc" | grep -cE '\bGIE\b|INTCON\s*[|=]' || true)
  has_flagclear=$(echo "$allsrc" | grep -cE '\b[A-Z0-9]+IF\s*=\s*0|bcf\s+[A-Z0-9]+,\s*[A-Z0-9]*IF' || true)

  for f in "${cfiles[@]}"; do
    s="$tmp/$(echo "$f" | tr '/' '_')"; strip "$f" > "$s"
    todos=$((todos + $(grep -c 'TODO' "$f" || true)))
    # C1 writing PORTx instead of LATx
    grep -nE '\bPORT[A-E](bits\.R[A-E][0-7])?\s*(=[^=]|\|=|&=|\^=|\+\+|--)' "$s" | while IFS=: read -r ln _; do
      warn "$f" "$ln" "Writing to PORTx does a read-modify-write on the PINS; drive outputs through LATx and read inputs from PORTx."; done
    findings=$((findings + $(grep -cE '\bPORT[A-E](bits\.R[A-E][0-7])?\s*(=[^=]|\|=|&=|\^=|\+\+|--)' "$s" || true)))
    # C2 __delay_ms without _XTAL_FREQ anywhere in the project
    if grep -qE '__delay_(ms|us)\s*\(' "$s" && [ "$has_xtal" -eq 0 ]; then
      ln=$(grep -nE '__delay_(ms|us)\s*\(' "$s" | head -1 | cut -d: -f1)
      warn "$f" "$ln" "__delay_ms/us used but _XTAL_FREQ is not defined anywhere in $proj; the delay length is undefined."; findings=$((findings+1))
    fi
    # C3 main() without an infinite loop
    if grep -qE '\bmain\s*\(' "$s" && ! grep -qE 'while\s*\(\s*(1|true)\s*\)|for\s*\(\s*;\s*;\s*\)' "$s"; then
      ln=$(grep -nE '\bmain\s*\(' "$s" | head -1 | cut -d: -f1)
      warn "$f" "$ln" "main() has no while(1)/for(;;) loop. On a microcontroller main() must never return."; findings=$((findings+1))
    fi
    # C4 assignment inside if/while condition
    grep -nE '\b(if|while)\s*\(\s*[A-Za-z_][A-Za-z0-9_.]*\s*=\s*[^=]' "$s" | while IFS=: read -r ln _; do
      warn "$f" "$ln" "Assignment (=) inside a condition; did you mean == ?"; done
    findings=$((findings + $(grep -cE '\b(if|while)\s*\(\s*[A-Za-z_][A-Za-z0-9_.]*\s*=\s*[^=]' "$s" || true)))
    # C5 interrupt plumbing
    if grep -qE '__interrupt' "$s"; then
      ln=$(grep -nE '__interrupt' "$s" | head -1 | cut -d: -f1)
      [ "$has_gie" -eq 0 ] && { warn "$f" "$ln" "An interrupt function exists but GIE is never set; it will never run."; findings=$((findings+1)); }
      [ "$has_flagclear" -eq 0 ] && { warn "$f" "$ln" "No interrupt flag (xxIF = 0) is cleared anywhere; the ISR will re-enter forever."; findings=$((findings+1)); }
      grep -nE '__delay_ms\s*\(' "$s" | head -1 | while IFS=: read -r ln2 _; do
        warn "$f" "$ln2" "__delay_ms in a file with an ISR: make sure the ISR itself never calls a blocking delay."; done
    fi
    # C6 pins read but ANSEL never configured
    if grep -qE '\bPORT[A-E](bits\.R[A-E][0-7])?\b' "$s" && [ "$has_ansel" -eq 0 ]; then
      ln=$(grep -nE '\bPORT[A-E]' "$s" | head -1 | cut -d: -f1)
      warn "$f" "$ln" "PORTx is read but no ANSELx is configured in $proj; analog-default pins always read 0."; findings=$((findings+1))
    fi
  done
  # C7 no TRIS at all
  if [ ${#cfiles[@]} -gt 0 ] && [ "$has_tris" -eq 0 ] && grep -qE '\b(LAT|PORT)[A-E]' <<<"$allsrc"; then
    warn "${cfiles[0]}" 1 "No TRISx assignment in $proj; pins default to inputs, so outputs will not drive."; findings=$((findings+1))
  fi

  for f in "${sfiles[@]}"; do
    s="$tmp/$(echo "$f" | tr '/' '_')"; strip_asm "$f" > "$s"
    todos=$((todos + $(grep -c 'TODO' "$f" || true)))
    # A1 ANSEL via access bank
    grep -nE '\b(clrf|setf|movwf|bcf|bsf|movff)\s+(BANKMASK\()?ANSEL[A-E]\)?\s*(,\s*[0-7]\s*)?(,\s*a\s*)?$' "$s" | grep -vE ',\s*b\s*$' | while IFS=: read -r ln _; do
      warn "$f" "$ln" "ANSELx (0xF38-0xF3C) is BELOW the access bank: this write silently lands in ordinary RAM and the pins stay analog. Use banksel ANSELx + BANKMASK(ANSELx),b then movlb 0."; done
    findings=$((findings + $(grep -nE '\b(clrf|setf|movwf|bcf|bsf|movff)\s+(BANKMASK\()?ANSEL[A-E]\)?\s*(,\s*[0-7]\s*)?(,\s*a\s*)?$' "$s" | grep -cvE ',\s*b\s*$' || true)))
    # A2 xc.inc
    if ! grep -qE '#include\s*<xc\.inc>' "$f"; then warn "$f" 1 "No #include <xc.inc>; register names and CONFIG will not resolve."; findings=$((findings+1)); fi
    # A3 retfie without fast-return
    grep -nE '^\s*retfie\s*$' "$s" | while IFS=: read -r ln _; do
      warn "$f" "$ln" "retfie without ',1' does not restore W/STATUS/BSR from the shadow registers; the interrupted code resumes with corrupted state."; done
    findings=$((findings + $(grep -cE '^\s*retfie\s*$' "$s" || true)))
    # A4 obsolete linker flags anywhere in the project
  done
  if grep -rqsE '\-p(resetVec|intVec)' "$proj" --include='*' 2>/dev/null; then
    warn "$proj" 1 "Obsolete -presetVec/-pintVec linker option found in $proj; the vectors are absolute psects now and these flags make the build fail."; findings=$((findings+1))
  fi

  total=$((total+findings)); todos_all=$((todos_all+todos))
  echo "| \`$proj\` | $findings | $todos |" >> "$SUMMARY"
  [ "$todos" -gt 0 ] && echo "::notice file=$proj::$proj still has $todos TODO marker(s)."
done
{ echo; echo "$total finding(s), $todos_all TODO marker(s) remaining. Lint never fails the build; it points at the usual suspects."; } >> "$SUMMARY"
echo "lint: $total finding(s), $todos_all TODO(s)"
exit 0
