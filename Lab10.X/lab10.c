/*
 * File:   lab10.c
 * Author: <your names here>
 *
 * Lab 10 - Speed Control of DC Motor Using PWM
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Controls DC fan speed using hardware PWM on CCP1 (RC2).
 *   The on-board button S1 (RB4) cycles through 6 duty-cycle steps:
 *   0% -> 20% -> 40% -> 60% -> 80% -> 100% -> repeat.
 *   The PWM output drives a TIP122 transistor which switches the fan.
 *
 * Pin Assignments:
 *   RB4        - On-board pushbutton S1 (active-low, has pull-up)
 *   RC2/CCP1   - PWM output - 1K - TIP122 base
 *
 * Motor Wiring (Delta AFB0405MA 5V fan):
 *   Red wire   - 5V rail
 *   Black wire - TIP122 collector
 *   Blue wire  - not connected (tachometer, unused)
 *   TIP122 emitter - GND
 *   1N4007 flyback diode across the fan (stripe toward 5V)
 *
 * Required PWM Configuration:
 *   Timer2 prescaler: 1:16
 *   PWM frequency: ~2 kHz
 *   PWM period = (PR2 + 1) * 4 * prescaler / FOSC
 *   -> work out the PR2 value that gives 2 kHz at FOSC = 16 MHz
 *   10-bit duty cycle range: 0 to (PR2+1)*4
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

/* Button Definition (on-board S1) */
#define BUTTON_PORT PORTBbits.RB4

/* Duty Cycle Steps (10-bit values)
 * TODO: With your PR2 value, the full period in duty-cycle units is
 *       (PR2+1)*4. Fill in the values for 0/20/40/60/80/100%. */
static const uint16_t dutyCycles[6] = {0, 0, 0, 0, 0, 0};
static const uint8_t  dutyPercent[6] = {0, 20, 40, 60, 80, 100};

static void init(void) {
    // TODO: 16 MHz HFINTOSC
    // TODO: ANSELB/ANSELC digital
    // TODO: RB4 input (button), RC2 output (PWM), latch low
}

/**
 * Configure CCP1 for PWM mode using Timer2 (~2 kHz).
 */
static void PWM_init(void) {
    // TODO: 1. PR2 = <your period value>
    //       2. CCPR1L = 0 and CCP1CONbits.DC1B = 0 (0% duty to start)
    //       3. CCP1CONbits.CCP1M = PWM mode (0b1100)
    //       4. Timer2: 1:16 prescaler, then enable TMR2ON
}

/**
 * Set the PWM duty cycle (10-bit value).
 * Upper 8 bits go in CCPR1L, lower 2 bits in CCP1CONbits.DC1B.
 */
static void PWM_set_duty(uint16_t duty) {
    // TODO
    (void)duty;
}

/**
 * Wait for a button press then release, debouncing both edges.
 */
static void wait_button_press_release(void) {
    // TODO: wait for press (LOW), delay ~30 ms,
    //       wait for release (HIGH), delay ~30 ms
}

void main(void) {
    init();
    PWM_init();

    uint8_t step = 0;   /* Start at 0% duty cycle */

    PWM_set_duty(dutyCycles[step]);

    while (1) {
        // TODO: on each press/release, advance to the next duty-cycle
        //       step (wrapping 5 -> 0) and update the PWM duty cycle
        (void)dutyPercent;
        (void)step;
    }
}
