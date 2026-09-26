#!/usr/bin/env bash
# Repository checks that do not need the compiler:
#   1. hygiene    - build artifacts / junk committed              (FAIL)
#   2. provided   - provided files modified (warn) or frozen files modified (FAIL)
#   3. lint       - common PIC18 mistakes in YOUR sources       (warnings)
#   4. authorship - who has committed to each lab                (info)
set -u
SUMMARY="${GITHUB_STEP_SUMMARY:-/dev/null}"
HERE="$(cd "$(dirname "$0")" && pwd)"
fail=0

# ---- 1. hygiene ----
echo "::group::Hygiene"
junk=$(git ls-files | grep -E '(^|/)(build|dist|debug|disassembly)/|(^|/)nbproject/private/|(^|/)nbproject/(Makefile-|Package-)|\.(hex|elf|o|p1|d|obj|cmf|hxl|sdb|sym|lst|rlf|map|pre|i|mk)$|(^|/)funclist$|(^|/)l\.obj$|(^|/)\.DS_Store$|(^|/)Thumbs\.db$' || true)
if [ -n "$junk" ]; then
  fail=1
  echo "$junk" | while read -r f; do echo "::error file=$f::Build artifact committed: $f. Run 'git rm --cached \"$f\"', commit, and keep .gitignore intact."; done
  echo "## Hygiene: FAIL" >> "$SUMMARY"; echo >> "$SUMMARY"; echo "Committed build artifacts:" >> "$SUMMARY"; echo "$junk" | sed 's/^/- `/; s/$/`/' >> "$SUMMARY"
else
  echo "no build artifacts committed"; echo "## Hygiene: OK" >> "$SUMMARY"
fi
[ -f .gitignore ] || { echo "::warning::.gitignore is missing - restore it from the starter repo."; }
big=$(git ls-files -z | xargs -0 -I{} sh -c 'sz=$(wc -c < "{}"); [ "$sz" -gt 2000000 ] && echo "{} ($sz bytes)"' 2>/dev/null || true)
[ -n "$big" ] && echo "$big" | while read -r l; do echo "::warning::Large file committed: $l"; done
echo "::endgroup::"

# ---- 2. provided / frozen files ----
echo "::group::Provided files"
provided_note=""
if [ -f .github/provided.sha256 ]; then
  changed=$(sha256sum -c --quiet .github/provided.sha256 2>&1 | sed -nE 's/^(.*): (FAILED.*|No such file.*)$/\1/p; s/^sha256sum: (.*): No such file.*/\1/p' | sort -u)
  if [ -z "$changed" ]; then
    echo "all provided files match the release"; provided_note="all provided files unmodified"
  else
    for f in $changed; do
      if [ -f .github/frozen.txt ] && grep -qxF "$f" .github/frozen.txt; then
        fail=1; echo "::error file=$f::FROZEN file modified or deleted: $f. This file is a contract for the lab and must be restored exactly. (git checkout <release commit> -- $f)"
        provided_note="$provided_note"$'\n'"- \`$f\` **FROZEN - modified (build fails)**"
      else
        echo "::warning file=$f::Provided file modified or deleted: $f. The lab did not ask you to change this; the grader compares it against the release."
        provided_note="$provided_note"$'\n'"- \`$f\` provided - modified (warning)"
      fi
    done
  fi
else
  echo "no manifest (.github/provided.sha256) - skipping"; provided_note="no manifest in this repo"
fi
{ echo; echo "## Provided files"; echo; echo "$provided_note"; } >> "$SUMMARY"
echo "::endgroup::"

# ---- 3. lint ----
echo "::group::PIC18 lint"
bash "$HERE/pic-lint.sh" || true
echo "::endgroup::"

# ---- 4. authorship ----
echo "::group::Authorship"
{ echo; echo "## Commits per lab"; echo; echo "| Project | Commits | Authors |"; echo "|---|---:|---|"; } >> "$SUMMARY"
for proj in *.X; do
  [ -d "$proj" ] || continue
  n=$(git rev-list --count HEAD -- "$proj" 2>/dev/null || echo 0)
  authors=$(git log --format='%an' -- "$proj" 2>/dev/null | grep -vE '^(JPL11|github-actions)' | sort | uniq -c | sort -rn | awk '{c=$1; $1=""; printf "%s (%d), ", substr($0,2), c}' | sed 's/, $//')
  na=$(git log --format='%an' -- "$proj" 2>/dev/null | grep -vE '^(JPL11|github-actions)' | sort -u | wc -l | tr -d ' ')
  student_commits=$(git log --format='%an' -- "$proj" 2>/dev/null | grep -cvE '^(JPL11|github-actions)' || true)
  echo "$proj: $student_commits student commit(s) by $na author(s): ${authors:-none yet}"
  if [ "$student_commits" -ge 6 ] && [ "$na" -le 1 ]; then
    echo "::notice file=$proj::$proj has $student_commits commits from a single author. Both partners must contribute commits - the history is part of the grade record."
  fi
  echo "| \`$proj\` | $n | ${authors:-–} |" >> "$SUMMARY"
done
echo "::endgroup::"
exit $fail
