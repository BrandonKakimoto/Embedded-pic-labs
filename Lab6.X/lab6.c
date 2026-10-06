/*
 * File:   lab6.c
 * Author: <your names here>
 *
 * Lab 6 - Analog-to-Digital Conversion Using PIC18F46K22
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Reads a 10k potentiometer on AN0 (RA0) using the 10-bit ADC.
 *   Displays the 10-bit result on 10 LEDs:
 *     PORTB (RB0-RB7) = lower 8 bits (ADRESL)
 *     PORTD (RD0-RD1) = upper 2 bits (ADRESH)
 *
 * Required ADC Configuration:
 *   Channel:       AN0
 *   Reference:     VDD and VSS
 *   Clock:         FOSC/32
 *   Acquisition:   8 TAD
 *   Justification: Right-justified
 *
 * Pin Assignments:
 *   RA0 (AN0) - Analog input (potentiometer wiper)
 *   RB0-RB7   - LEDs (bits 0-7 of ADC result, LSB)
 *   RD0-RD1   - LEDs (bits 8-9 of ADC result, MSB)
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

static void init(void) {
    // TODO: 16 MHz HFINTOSC

    // -- Port Configuration --
    // TODO: RA0 as INPUT and ANALOG (TRISAbits, ANSELAbits)
    // TODO: PORTB all digital outputs, LEDs off
    // TODO: PORTD all digital outputs, LEDs off

    // -- ADC Configuration --
    // TODO: ADCON0 - select channel AN0, turn the ADC module ON
    // TODO: ADCON1 - positive reference = VDD, negative reference = VSS
    // TODO: ADCON2 - right-justified, 8 TAD acquisition, FOSC/32 clock
    // (Datasheet chapter 17 has the field encodings.)
}

void main(void) {
    init();

    uint16_t adcResult;

    while (1) {
        // TODO: 1. Start a conversion (ADCON0bits.GO = 1)
        //       2. Wait until the GO bit clears
        //       3. Clear the ADIF flag
        //       4. Combine ADRESH:ADRESL into a 10-bit result
        //       5. Lower 8 bits -> LATB, upper 2 bits -> LATD
        //       6. Delay ~100 ms before the next sample
        adcResult = 0;
        (void)adcResult;
    }
}
