# What Lab CI checks in `Lab2.X`

Every push to `main` runs these automatically. Open the run in the **Actions** tab: the
checklist and the warnings below appear in the run summary and as annotations on your lines.

## 1. It must compile

`xc8-cc -Wall` (or `pic-as` for assembly) exactly as the grader builds it. A red X here is the
only thing that fails the run. Warnings in *your* files are listed with file and line.

## 2. Required structures (the grader's static score)

The run summary shows this list with a check mark for each one it finds in your code.
Comments do not count. These are the same patterns the grader scores.

- [ ] Timer0 configured (T0CON written)
- [ ] Random generator seeded (srand)
- [ ] PORTB switched to digital (ANSELB)

## 3. Mistakes this lab is known for

Warnings only: they never turn the check red, but each one has cost a pair real hours on the bench.
The id in brackets is what you will see on the annotation.

- **[L2-pll]** (warns where found) The 64 MHz PLL has been unreliable on the lab boards this term (delays drift, random lockups). Run at 16 MHz: leave PLLEN off and set _XTAL_FREQ 16000000.
- **[L2-xtal]** (warns where found) _XTAL_FREQ must match the clock you actually set in OSCCON (16000000 for the 16 MHz HFINTOSC), or every __delay_ms is the wrong length.
- **[L2-anselb]** (warns if missing) RB4 (the button) is on PORTB, which wakes up ANALOG because PBADEN=ON. Without ANSELB cleared the button reads 0 forever, i.e. always pressed.
- **[L2-patterns]** (warns where found) The dice LED pattern table is still all 0x00, so no die face can ever light.
- **[L2-seed]** (warns if missing) The random seed must come from Timer0 at the moment of the press (TMR0L into srand). A fixed seed rolls the same sequence every boot.
- **[L2-debounce]** (warns if missing) No ~30 ms debounce delay found. One physical press will register as several rolls.

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

## What CI cannot see

Timing, wiring, display polarity, a dead breadboard row, the wrong chip in the socket.
A green check means it builds. Behavior is checked on hardware at check-off.
