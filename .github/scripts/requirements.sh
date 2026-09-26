#!/usr/bin/env bash
# Shared requirements check used by BOTH Lab CI and the grader, so they
# can never disagree. Matches against code only (comments stripped) in
# files the student owns (provided files excluded).
#
# Usage: requirements.sh <LabDir> <requirements.txt> [provided.sha256]
# Prints one line per requirement:  PASS<TAB>label  or  FAIL<TAB>label
# Exit code = number of FAILs (0 = all met; also 0 if no requirements).
set -u
DIR="${1:?lab dir}"; REQ="${2:?requirements.txt}"; PROV="${3:-}"
lab=$(basename "$DIR")
provided=""
[ -n "$PROV" ] && [ -f "$PROV" ] && provided=$(awk '{print $2}' "$PROV" | sed "s#^$lab/##")

codefile=$(mktemp); trap 'rm -f "$codefile"' EXIT
for f in "$DIR"/*.c "$DIR"/*.h "$DIR"/*.S; do
  [ -f "$f" ] || continue
  printf '%s\n' "$provided" | grep -qxF "$(basename "$f")" && continue
  if [ "${f##*.}" = "S" ]; then
    sed -E 's/;.*$//' "$f" >> "$codefile"
  else
    perl -0777 -pe 's{/\*.*?\*/}{ }gs; s{//[^\n]*}{}g' "$f" >> "$codefile"
  fi
done
fails=0
while IFS=$'\t' read -r label pattern; do
  [ -n "$label" ] || continue
  if grep -qE -- "$pattern" "$codefile"; then
    printf 'PASS\t%s\n' "$label"
  else
    printf 'FAIL\t%s\n' "$label"; fails=$((fails+1))
  fi
done < <(grep -vE '^\s*(#|$)' "$REQ" | awk -F'\t' -v L="$lab" '$1==L {print $2 "\t" $3}')
exit $fails
