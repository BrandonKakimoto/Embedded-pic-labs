# What Lab CI checks in `Lab11.X`

Every push to `main` runs these automatically. Open the run in the **Actions** tab: the
checklist and the warnings below appear in the run summary and as annotations on your lines.

## 1. It must compile

`xc8-cc -Wall` (or `pic-as` for assembly) exactly as the grader builds it. A red X here is the
only thing that fails the run. Warnings in *your* files are listed with file and line.

## 2. Required structures (the grader's static score)

The run summary shows this list with a check mark for each one it finds in your code.
Comments do not count. These are the same patterns the grader scores.

- [ ] MSSP configured for I2C (SSPCON1)
- [ ] I2C baud rate set (SSPADD)
- [ ] I2C data buffer used (SSPBUF)

## 3. Mistakes this lab is known for

Warnings only: they never turn the check red, but each one has cost a pair real hours on the bench.
The id in brackets is what you will see on the annotation.

- **[L11-smp]** (warns if missing) SSPSTATbits.SMP = 1 (slew-rate control off) for 100 kHz. Without it the bus timing is for 400 kHz.
- **[L11-tris]** (warns if missing) RC3 (SCL) and RC4 (SDA) must BOTH be configured as INPUTS for the MSSP to drive them.
- **[L11-anselc]** (warns if missing) SCL/SDA on RC3/RC4 must be digital (ANSELC cleared).
- **[L11-sen]** (warns if missing) No start condition (SSPCON2bits.SEN).
- **[L11-pen]** (warns if missing) No stop condition (SSPCON2bits.PEN); the bus is never released.
- **[L11-rcen]** (warns if missing) No receive enable (RCEN); nothing can be read from the sensor.
- **[L11-ack]** (warns if missing) No ACK/NACK handling (ACKDT/ACKEN) after a received byte.
- **[L11-wait]** (warns if missing) I2C_wait must spin while (SSPCON2 & 0x1F) or R_nW is set; the 0x1F mask is missing.

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
