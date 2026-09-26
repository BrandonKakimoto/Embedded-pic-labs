/*
 * File:    lab3.c
 * Author: Brandon Kakimoto & Jason Lopez
 *
 * Lab 3 - Traffic Light Controller
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Pin Assignments:
 *    PORTD - South (RD5=G, RD4=Y, RD3=R) and North (RD2=G, RD1=Y, RD0=R)
 *    PORTB - East  (RB5=G, RB4=Y, RB3=R) and West  (RB2=G, RB1=Y, RB0=R)
 *    RB6/RB7 must be left untouched (PGC/PGD for programming with LVP=ON)
 *
 * Traffic Sequence:
 *    State A: EW Green,  NS Red     - 6 seconds
 *    State B: EW Yellow, NS Red     - 3 seconds
 *    State C: NS Green,  EW Red     - 6 seconds
 *    State D: NS Yellow, EW Red     - 3 seconds
 */

#include <xc.h>
#include <stdint.h>
#include "Configuration.h"

#define _XTAL_FREQ 16000000UL

static void init(void) {
    // TODO: Configure the oscillator for 64 MHz
    //       (16 MHz HFINTOSC via OSCCONbits.IRCF/SCS + 4x PLL, same as Lab 2)
    OSCCONbits.IRCF = 0b111; // Set the internal clock speed to 16 MHz
    OSCCONbits.SCS  = 0b00;  // Select internal oscillator block
    OSCTUNEbits.PLLEN = 1;   // Turn on 4x multiplier to boost total speed to 64 MHz

    // TODO: Make PORTB and PORTD digital (ANSELB / ANSELD).
    //       Critical on the K22: PBADEN=ON makes PORTB analog at reset!
    ANSELB = 0x00; // Turn off analog mode so PORTB pins work as simple digital ON/OFF switches
    ANSELD = 0x00; // Turn off analog mode on PORTD pins

    // TODO: Make RB0-RB5 and RD0-RD5 outputs WITHOUT touching bits 6-7.
    //       Hint: TRISB &= 0xC0 clears only the lower six bits.
    TRISB &= 0xC0; // Set RB0-RB5 as outputs (0 = output) while leaving RB6-RB7 untouched
    TRISD &= 0xC0; // Set RD0-RD5 as outputs (0 = output) while leaving RD6-RD7 untouched

    // TODO: Start with all traffic LEDs off (again, preserve bits 6-7).
    LATB &= 0xC0;  // Send 0V to RB0-RB5 so all PORTB LEDs start turned OFF
    LATD &= 0xC0;  // Send 0V to RD0-RD5 so all PORTD LEDs start turned OFF
}

/**
 * Update traffic lights using only the lower 6 bits of each port.
 * Upper 2 bits (RB6/RB7, RD6/RD7) must be preserved.
 */
static void set_lights(unsigned char rd_val, unsigned char rb_val) {
    // TODO: Write rd_val to LATD and rb_val to LATB, making so that
    //       only bits 0-5 change. Hint: (LATD & 0xC0) | (rd_val & 0x3F)
    LATD = (LATD & 0xC0) | (rd_val & 0x3F);     // Keep top 2 programmer pins intact and update lower 6 LED pins on PORTD
    LATB = (LATB & 0xC0) | (rb_val & 0x3F);     // Keep top 2 programmer pins intact and update lower 6 LED pins on PORTB
}

void main(void) {
    init();

    while (1) {
        // TODO: Implement the four-state sequence with set_lights() and
        //       __delay_ms(). Work out the PORTD/PORTB bit patterns for
        //       each state from the pin assignment table above.
        //
        // State A: EW Green,  NS Red  - 6 s
        // State B: EW Yellow, NS Red  - 3 s
        // State C: NS Green,  EW Red  - 6 s
        // State D: NS Yellow, EW Red  - 3 s

        // State A: EW Green (RB5, RB2 HIGH = 0x24), NS Red (RD3, RD0 HIGH = 0x09)
        set_lights(0x09, 0x24);
        __delay_ms(6000);   // Call four 6000ms delays to equal 24,000ms total
        __delay_ms(6000);   // Due to 4x clock speedup, 24,000ms = 6 real seconds 
        __delay_ms(6000);
        __delay_ms(6000);

        // State B: EW Yellow (RB4, RB1 HIGH = 0x12), NS Red (RD3, RD0 HIGH = 0x09) 
        set_lights(0x09, 0x12);
        __delay_ms(3000);   // Call four 3000ms delays to equal 12,000ms total
        __delay_ms(3000);   // Due to 4x clock speedup, 12,000ms = 3 real seconds
        __delay_ms(3000);
        __delay_ms(3000);

        // State C: NS Green (RD5, RD2 HIGH = 0x24), EW Red (RB3, RB0 HIGH = 0x09)
        set_lights(0x24, 0x09);
        __delay_ms(6000);   // Delay for 6 real seconds
        __delay_ms(6000);
        __delay_ms(6000);
        __delay_ms(6000);

        // State D: NS Yellow (RD4, RD1 HIGH = 0x12), EW Red (RB3, RB0 HIGH = 0x09) 
        set_lights(0x12, 0x09);
        __delay_ms(3000);   // Delay for 3 real seconds
        __delay_ms(3000);
        __delay_ms(3000);
        __delay_ms(3000);
    }
}
