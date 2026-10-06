# What Lab CI checks in `Lab5p1.X`

Every push to `main` runs these automatically. Open the run in the **Actions** tab: the
checklist and the warnings below appear in the run summary and as annotations on your lines.

## 1. It must compile

`xc8-cc -Wall` (or `pic-as` for assembly) exactly as the grader builds it. A red X here is the
only thing that fails the run. Warnings in *your* files are listed with file and line.

## 2. Required structures (the grader's static score)

The run summary shows this list with a check mark for each one it finds in your code.
Comments do not count. These are the same patterns the grader scores.

- (none defined for this lab)

## 3. Mistakes this lab is known for

Warnings only: they never turn the check red, but each one has cost a pair real hours on the bench.
The id in brackets is what you will see on the annotation.

- **[L5a-ansel]** (warns if missing) The LCD pins live on PORTA/PORTD; both ANSELA and ANSELD must be cleared or the LCD shows nothing.
- **[L5a-include-c]** (warns where found) Include LCD.h, not LCD.c. CI compiles LCD.c as its own file, so including it gives duplicate symbols.
- **[L5a-lcdinit]** (warns if missing) The LCD is never initialised (LCD_init). The provided driver needs it before any write.

## Generic warnings on every lab

- **[C1]** Writing to PORTx instead of LATx (read-modify-write on the pins).
- **[C2]** __delay_ms/us used but _XTAL_FREQ not defined.
- **[C3]** main() has no while(1) loop.
- **[C4]** Assignment (=) inside an if/while condition.
- **[C5]** An interrupt function exists but GIE is never set.
- **[C6]** No interrupt flag (xxIF = 0) is ever cleared.
- **[C7]** PORTx read but no ANSELx configured (analog pins read 0).
- **[C8]** No TRISx assignment at all.
- **[A1]** Assembly: ANSELx written through the access bank (,a) instead of banksel + ,b.
- **[A2]** Assembly: no #include <xc.inc>.
- **[A3]** Assembly: retfie without ,1.
- **[A4]** Obsolete -presetVec/-pintVec linker flags present.

## Names and places CI also checks

- A source file named `bios.s` (lower case) is **not assembled**; it must be `bios.S`.
- A file name with spaces or brackets is ignored by the build entirely.
- `WRITEUP.md`, where a lab asks for one, belongs **inside this lab folder**, not at the top of the repository.

Each of these produces a warning on the run. They are the mistakes that cost points for reasons that have nothing to do with your code.

## What CI cannot see

Timing, wiring, display polarity, a dead breadboard row, the wrong chip in the socket.
A green check means it builds. Behavior is checked on hardware at check-off.
