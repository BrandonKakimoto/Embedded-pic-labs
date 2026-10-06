/*
 * File:   lab7.c
 * Author: <your names here>
 *
 * Lab 7 - ADC Measurement Display Using LCD
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Extends Lab 6 with an interrupt-driven ADC and an LCD display.
 *   - ADC reads the 10k pot on AN0 (RA0) using interrupts
 *   - 10-bit binary result displayed on LEDs
 *   - Decimal ADC value and calculated voltage on the LCD
 *
 * Pin Assignments:
 *   RA0 (AN0)  - Analog input (potentiometer)
 *   RD0        - LCD RS
 *   RD2        - LCD EN
 *   RA4-RA7    - LCD D4-D7
 *   RB0-RB7    - LEDs (ADC bits 0-7, lower 8 bits)
 *   RC0-RC1    - LEDs (ADC bits 8-9, upper 2 bits)
 *
 * ADC Configuration (same as Lab 6, plus interrupts):
 *   Channel AN0, VDD/VSS reference, FOSC/32, 8 TAD, right-justified
 *
 * Voltage Calculation:
 *   Voltage = (ADC Value / 1023) * 5.0V
 *
 * LCD driver (LCD.c / LCD.h) is PROVIDED - do not modify LCD.c.
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"
#include "LCD.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

/* Global ADC result (updated in the ISR) */
static volatile uint16_t adcResult = 0;
static volatile uint8_t  adcReady  = 0;     /* Flag: new result available */

/* Interrupt Service Routine */
void __interrupt() ISR(void) {
    // TODO: If the ADC interrupt flag (PIR1bits.ADIF) is set:
    //   1. Read ADRESH:ADRESL into adcResult
    //   2. Set adcReady = 1
    //   3. Clear ADIF
    //   4. Start the next conversion (GO = 1)
}

static void init(void) {
    // TODO: 16 MHz HFINTOSC

    // -- Port Configuration --
    // TODO: RA0 analog input; PORTB digital outputs (LEDs);
    //       RC0/RC1 digital outputs (LEDs); ANSELD = 0 for the LCD pins.
    //       (RA4-RA7, RD0, RD2 directions are handled by LCD_init.)

    // -- ADC Configuration (same as Lab 6) --
    // TODO: ADCON0 / ADCON1 / ADCON2

    // -- Interrupt Configuration --
    // TODO: clear ADIF, then enable in this order:
    //       ADIE (peripheral source), PEIE (peripheral group), GIE (global)
}

void main(void) {
    init();
    LCD_init();

    // TODO: Write static labels: line 1 "ADC:", line 2 "Volt:"

    // TODO: Start the FIRST conversion here (the ISR keeps it going)

    while (1) {
        if (adcReady) {
            adcReady = 0;

            // TODO: 1. Copy adcResult to a local variable
            //       2. Lower 8 bits -> LATB; bits 8-9 -> LATC0/LATC1
            //       3. voltage = (adc / 1023.0) * 5.0
            //       4. LCD line 1: print adc (LCD_write_variable, width 4)
            //       5. LCD line 2: print voltage (LCD_write_float, width 4,
            //          2 decimals) followed by 'V'
            //       6. Delay ~100 ms (about a 10 Hz update rate)
        }
    }
}
