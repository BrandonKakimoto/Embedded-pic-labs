# Embedded Systems with the PIC18F46K22

C programs I wrote for a small microcontroller, the **PIC18F46K22**. They start with turning LEDs on and off and build up to using displays, reading analog voltages, handling interrupts, and reading a digital sensor.

Built for **ECE 3301L – Introduction to Microcontrollers Lab** at **Cal Poly Pomona**.

---

## What I Learned

| Skill | In plain terms |
|---|---|
| **Inputs and outputs** | Reading buttons and turning LEDs on and off by setting the chip's pins |
| **State machines** | Writing a program as a set of "modes" with clear rules for moving between them (e.g., green → yellow → red) |
| **Displays** | Showing text on an LCD screen and numbers on seven-segment displays |
| **Analog-to-digital conversion (ADC)** | Turning a voltage, like a knob's position, into a number the program can use |
| **Interrupts** | Letting the hardware tell the program when something is ready, instead of the program checking over and over |
| **Sensors** | Reading a temperature and humidity sensor by following its datasheet |

---

## Tools & Hardware

- **Microcontroller:** PIC18F46K22
- **Board:** Microchip Curiosity Development Board
- **Software:** MPLAB X IDE with the XC8 C compiler

---

## Projects

| # | Project | One-line summary |
|---|---|---|
| 2 | [Dice Roller](#lab-2--dice-roller) | Press a button, roll two LED dice |
| 3 | [Traffic Light](#lab-3--traffic-light) | A timed traffic light for a four-way intersection |
| 4 | [Traffic Light with Emergency & Crosswalk](#lab-4--traffic-light-with-emergency--crosswalk) | Adds an emergency stop button and a crosswalk countdown |
| 5 | [LCD & Seven-Segment Displays](#lab-5--lcd--seven-segment-displays) | Shows counters on two kinds of displays |
| 6 | [Reading a Knob (ADC)](#lab-6--reading-a-knob-adc) | Reads a knob's position and shows it on LEDs |
| 7 | [Digital Voltmeter](#lab-7--digital-voltmeter) | Measures voltage and shows it on the LCD, using interrupts |
| 8 | [Temperature & Humidity Monitor](#lab-8--temperature--humidity-monitor) | Reads a DHT11 sensor and shows the results |

---

### Lab 2 – Dice Roller
**What it does:** Two groups of LEDs act like dice. Pressing a button picks two random numbers from 1 to 6 and lights up the matching dice pattern for 3 seconds.

**How it works:** A lookup table stores which LEDs to turn on for each number, so the program doesn't need a separate `if` statement for every face. The program also waits for the button to be released so one press doesn't count as several rolls (this is called *debouncing*).

### Lab 3 – Traffic Light
**What it does:** Controls 12 LEDs as traffic lights for North, South, East, and West. East–West gets green for 6 seconds, then yellow for 3, then North–South gets the same.

**How it works:** The program is a *state machine*: each step of the light cycle is a state, and the program moves through them in order. It starts with all lights red so it's always in a safe state, and two directions can never be green at the same time.

### Lab 4 – Traffic Light with Emergency & Crosswalk
**What it does:** Builds on Lab 3 with two buttons:
- **Emergency stop:** flashes every light red right away. It overrides everything else.
- **Crosswalk button:** the program remembers the press and waits for a safe moment in the cycle, then counts down 9 to 0 on a seven-segment display.

**How it works:** The main idea is *priority*. The emergency button always wins. The crosswalk request is saved and only handled when it's safe, the same way a real crosswalk button works.

### Lab 5 – LCD & Seven-Segment Displays
**What it does:**
- **Part 1:** A 16x2 LCD screen counts from 0 to 15 and keeps track of how many times it has started over.
- **Part 2:** Two seven-segment digits count seconds from 00 to 59.

**How it works:** The two digits share the same wires. The program lights one digit, then the other, switching so fast your eye sees both on at once (this is called *multiplexing*). It saves pins on the chip.

### Lab 6 – Reading a Knob (ADC)
**What it does:** Reads the position of a knob (potentiometer) and shows the value in binary on 10 LEDs.

**How it works:** The chip's ADC turns the knob's voltage into a number from 0 to 1023. The program starts a conversion, waits for it to finish, and copies the result to the LEDs.

### Lab 7 – Digital Voltmeter
**What it does:** Same as Lab 6, but it also calculates the actual voltage (0–5 V) and shows both the raw number and the voltage on the LCD.

**How it works:** Instead of waiting in a loop for the ADC, the program uses an *interrupt*: the ADC signals when it's done, and a special function handles the result right away. The voltage is calculated as `value ÷ 1023 × 5 V`. I also had to move some LEDs to different pins because the LCD needed them.

### Lab 8 – Temperature & Humidity Monitor
**What it does:** Reads temperature and humidity from a DHT11 sensor and shows them on the LCD.

**How it works:** The sensor sends data over a single wire, and each 0 or 1 is encoded by how long the signal stays high. Following the datasheet, the program sends a "start" signal, then times each pulse to read 40 bits. It checks the last byte (a *checksum*) to make sure the data arrived correctly.

---

## How to Run

1. Install [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide) and the [XC8 compiler](https://www.microchip.com/mplab/compilers).
2. Open a lab folder as a project in MPLAB X, or create a new **PIC18F46K22** project and add that lab's files.
3. Connect the Curiosity board, then build and upload.
4. Wire the parts as described in that lab.

---

## Folder Layout

```
.
├── Lab02_Dice/
├── Lab03_TrafficLight/
├── Lab04_TrafficLight_EmergencyPedestrian/
├── Lab05_LCD_SevenSegment/
├── Lab06_ADC_Polling/
├── Lab07_ADC_Interrupt_LCD/
├── Lab08_DHT11/
└── README.md
```

---

## Author

**Brandon Kakimoto**, Cal Poly Pomona
[GitHub](https://github.com/BrandonKakimoto)

> Course projects for ECE 3301L. The lab instructions came from the course. The code is my own.
