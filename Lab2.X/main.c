/*
 * File:   main.c
 * Author: Brandon Kakimoto & Jason Lopez
 *
 * Lab 2 - Electronic Dice
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Press the button (RB4, active-low) to "roll" two dice.
 *   Each die is shown on 7 LEDs (PORTC = die 1, PORTD = die 2)
 *   using the dice patterns from the lab handout.
 *   Timer0 is used as a random seed source (human press timing).
 */

#include <xc.h>
#include <stdlib.h>
#include <stdint.h>

#include "Configuration.h"

#define _XTAL_FREQ 64000000UL

/* Dice LED bit patterns - fill these in from the dice table
 * in the lab handout. Index 0 = face "1", index 5 = face "6". */
static const uint8_t dicePatterns[6] = {
    0x08, // Index 0 -> Roll 1 (Center LED)
    0x22, // Index 1 -> Roll 2 (2 Diagonal Corner LEDs)
    0x2A, // Index 2 -> Roll 3 (2 Diagonal Corner LEDs + Center)
    0x55, // Index 3 -> Roll 4 (4 Corner LEDs)
    0x5D, // Index 4 -> Roll 5 (4 Corner LEDs + Center)
    0x77  // Index 5 -> Roll 6 (All 6 Outer LEDs)
};

static void init_clock_to_64MHz(void) {
    // TODO: Select the 16 MHz HFINTOSC using OSCCONbits.IRCF
    OSCCONbits.IRCF = 0b111;

    // TODO: Select the internal oscillator block using OSCCONbits.SCS
    OSCCONbits.SCS = 0b00;

    // TODO: Enable the 4x PLL (OSCTUNEbits.PLLEN) -> 64 MHz
    OSCTUNEbits.PLLEN = 1;
}

static void init_timer0_free_running(void) {
    // TODO: Configure Timer0 as a free-running 16-bit timer:
    //   - internal clock (Fosc/4)
    //   - no prescaler
    //   - clear TMR0H/TMR0L, then turn the timer ON
    // Hint: use the T0CONbits fields. This timer is your entropy source
    T0CONbits.T08BIT = 0; // 16-bit timer
    T0CONbits.T0CS   = 0; // Sets the timer clock source to the internal instruction clock (Fosc/4)
    T0CONbits.PSA    = 1; // Disables the prescaler (a 1:1 clock ratio) so the timer counts up at full speed

    TMR0H = 0x00; // Clears the high byte of the counter to zero
    TMR0L = 0x00; // Clears the low byte of the counter to zero

    T0CONbits.TMR0ON = 1; // Turns Timer0 on so it starts counting continuously in the background
}

static void init_io(void) {
    // TODO: Make PORTC and PORTD outputs (dice LEDs) and clear their latches.
    TRISC = 0x00; // Sets all PORTC pins as OUTPUTS (for Die #1 LEDs)
    TRISD = 0x00; // Sets all PORTD pins as OUTPUTS (for Die #2 LEDs)
    LATC  = 0x00; // Clears PORTC latch so Die #1 LEDs start off
    LATD  = 0x00; // Clears PORTD latch so Die #2 LEDs start off

    // TODO: Make RB4 an input (button, active-low).
    TRISBbits.TRISB4 = 1; // Sets pin RB4 as an INPUT (connected to S1 button)

    // TODO: PBADEN=ON means PORTB<5:0> wake up as ANALOG pins
    //       Force RB4 digital (ANSELB) or the button will always read 0
    ANSELBbits.ANSB4 = 0; // Disables analog mode on RB4, making it read digital inputs (0 or 1)

    // Enable internal weak pull-up on RB4 to ensure high logic state when untouched
    INTCON2bits.RBPU = 0; // Enables global weak pull-up resistors on PORTB 
    WPUBbits.WPUB4   = 1; // Turns on the specific internal weak pull-up resistor on pin RB4 to hold it at 5V when unpressed
}

static uint8_t read_button_pressed(void) {
    // TODO: return 1 while the button is pressed (remember: active-low)
    if (PORTBbits.RB4 == 0) { // Returns 1 if RB4 is pressed and 0 if RB4 is unpressed
        return 1;
    } else {
        return 0;
    } 
}

static void debounce_press(void) {
    // TODO: short delay (~30 ms) to ride out contact bounce
    __delay_ms(30); // Pauses execution for 30 ms to let physical button contact vibrations settle down
}

static void wait_for_release(void) {
    // TODO: block until the button is released, then debounce the release
    while (PORTBbits.RB4 == 0) { // Creates a loop that stays active as long as RB4 is held low
        __delay_ms(10); // Forces the microcontroller to check the pin at a controlled rate
    }
    __delay_ms(50); // Waits 50 ms to ride out electrical noise when releasing the switch
}

void main(void) {
    init_clock_to_64MHz(); // Executes clock initialization
    init_io(); // Executes I/O pin directions and settings
    init_timer0_free_running(); // Starts Timer0

    while (1) {
        // TODO: When a (debounced) press is detected:
        if (read_button_pressed()) { // Checks if S1 button is pressed down
            debounce_press(); // Waits out mechanical contact bounce

            if (read_button_pressed()) { // Re-verifies button is still held down after 30 ms
                // 1. Read TMR0H:TMR0L and use it to seed rand() with srand()
                uint16_t seed = TMR0L; // Grab the low part of the timer
                seed |= ((uint16_t)TMR0H << 8); // Combine it with the high part for a full 16-bit value
                srand(seed); // Seed the randomizer so every roll is unpredictable

                // 2. Roll two dice with rand() % 6
                uint8_t die1 = rand() % 6; // Calculates a pseudo-random index number from 0 to 5 for Die #1
                uint8_t die2 = rand() % 6; // Calculates a pseudo-random index number from 0 to 5 for Die #2

                // 3. Show the two patterns on LATC and LATD
                LATC = dicePatterns[die1]; // Sends the matching LED pattern to PORTC outputs
                LATD = dicePatterns[die2]; // Sends the matching LED pattern to PORTD outputs

                // 4. Keep them displayed for 3 seconds, then turn LEDs off
                __delay_ms(3000); // Holds LED output active on breadboard for 3 full seconds
                LATC = 0x00; // Turns PORTC LEDs off
                LATD = 0x00; // Turns PORTD LEDs off

                // 5. Wait for the button to be released (one roll per press)
                wait_for_release(); // Stops program execution until you release button S1 before accepting the next roll.
            }
        }
    }
}