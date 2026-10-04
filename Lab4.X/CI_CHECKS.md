# What Lab CI checks in `Lab4.X`

Every push to `main` runs these automatically. Open the run in the **Actions** tab: the
checklist and the warnings below appear in the run summary and as annotations on your lines.

## 1. It must compile

`xc8-cc -Wall` (or `pic-as` for assembly) exactly as the grader builds it. A red X here is the
only thing that fails the run. Warnings in *your* files are listed with file and line.

## 2. Required structures (the grader's static score)

The run summary shows this list with a check mark for each one it finds in your code.
Comments do not count. These are the same patterns the grader scores.

- [ ] State machine implemented with switch/case
- [ ] Seven-segment output routine (sevenSeg)
- [ ] Pedestrian request handled (pedRequest)
- [ ] Traffic states enumerated (TrafficState)

## 3. Mistakes this lab is known for

Warnings only: they never turn the check red, but each one has cost a pair real hours on the bench.
The id in brackets is what you will see on the annotation.

- **[L4-pgc]** (warns where found) A whole-register write to TRISB/LATB also drives RB6/RB7 (PGC/PGD). Mask bits 6-7 as in Lab 3.
- **[L4-segtable]** (warns where found) The sevenSeg table is still all 0xDF, so every digit shows the same pattern.
- **[L4-cathode]** (warns where found) This is a common-CATHODE segment table (active-high). The kit display is common-ANODE: segments light on 0, so the table is the inverse (0 shows as 0xC0/0xDF-style values). Hours have been lost to this.
- **[L4-xtal]** (warns where found) _XTAL_FREQ must match OSCCON (16000000 at 16 MHz) or the countdown timing is wrong.
- **[L4-rc5]** (warns if missing) RC5 is the S2 input on the same port as the segments. Writing all of LATC disturbs it; mask with 0x20.
- **[L4-re0]** (warns if missing) The pedestrian button is RE0. Without ANSELE cleared it reads 0; without reading RE0 the pedestrian request never happens.

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
