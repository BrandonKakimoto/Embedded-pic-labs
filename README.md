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

4. Every push runs **Lab CI** — check the **Actions** tab. Two jobs:

   - **Compile** builds every project exactly as the grader does
     (`xc8-cc -Wall` / `pic-as`). A green check means every project
     assembles, links and produces firmware. Compiler warnings in *your*
     files show up as annotations on the run, with file and line.
     The run's **Artifacts** section has every `firmware.hex` it built,
     so you can flash a CI-built hex if your local toolchain misbehaves.
   - **Repository checks** fails the run if build artifacts
     (`build/`, `dist/`, `*.hex`, `*.p1`, …) are committed or if a
     *frozen* contract file (for example `uart.h` in Lab 16) is changed.
     It warns if a provided library file (`LCD.c`, config headers …)
     is modified, and runs a lint that points at classic PIC18 mistakes:
     writing `PORTx` instead of `LATx`, `__delay_ms` without
     `_XTAL_FREQ`, an ISR with no `GIE` or no flag clear, `ANSELx`
     written through the access bank in assembly, `main()` without a
     `while(1)`, `=` inside an `if`. The summary also counts the
     `TODO` markers you have left and shows who has committed to each lab.

   **A green check is required but does not by itself mean the lab is
   correct** — behavior is graded on hardware in lab. Warnings never
   block the check, but read them: most of them are bugs you have not
   met yet.

5. To submit: copy the URL of your final commit
   (repository page → Commits → the one you want) and paste it into the
   Canvas assignment. One submission per pair.

## Rules

- Both partners must contribute commits. Commit history is part of the
  grade record.
- Do not modify anything under `.github/`, the provided library files
  (`LCD.c`, `OLED.c`), or the configuration headers unless the lab says
  to. CI compares them against the release.
- Do not commit MPLAB `build/`, `dist/`, or `debug/` folders (the
  `.gitignore` already prevents this).
- The work must be your team's own. Copying another team's repository
  is an academic-integrity violation, and commit histories make it
  easy to detect.
