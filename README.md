# ECE 3301L — Microcontrollers Lab (Pair Repository)

This is your team's repository for the whole semester. A new `LabN.X`
folder appears here each week when the lab is released — `git pull` at
the start of each lab session.

## Workflow

1. `git pull` to get the newest lab folder.
2. Open the `LabN.X` project in MPLAB X and complete every `TODO` in the
   source file. Lab 1 is provided complete as a worked example.
3. Commit and push as you work:

   ```bash
   git add LabN.X
   git commit -m "Lab N: working LED sequence"
   git push
   ```

4. Every push is compiled automatically — check the **Actions** tab.
   A green check means your project compiles and produces firmware.
   A red X means it does not; click into the run to see the compiler
   errors. **A green check is required but does not by itself mean the
   lab is correct** — behavior is graded on hardware in lab.
5. To submit: copy the URL of your final commit
   (repository page → Commits → the one you want) and paste it into the
   Canvas assignment. One submission per pair.

## Rules

- Both partners must contribute commits. Commit history is part of the
  grade record.
- Do not modify `.github/workflows/`, provided library files
  (`LCD.c`), or the configuration headers unless the lab says to.
- Do not commit MPLAB `build/`, `dist/`, or `debug/` folders (the
  `.gitignore` already prevents this).
- The work must be your team's own. Copying another team's repository
  is an academic-integrity violation, and commit histories make it
  easy to detect.
