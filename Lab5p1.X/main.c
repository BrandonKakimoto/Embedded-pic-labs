/*
 * File:   main.c
 * Author: <your names here>
 *
 * Lab 5 Part 1 - LCD Interface + Counter (0-15) + Round
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Displays a counter (0-15) and a Round variable on a character LCD.
 *   The counter increments every second and resets to 0 after 15.
 *   Round increments each time the counter resets to 0.
 *
 * LCD Pin Assignments (from Figure 1):
 *   RD0 = RS
 *   RD2 = EN
 *   RA4 = D4
 *   RA5 = D5
 *   RA6 = D6
 *   RA7 = D7
 *
 * The LCD driver (LCD.c / LCD.h) is PROVIDED - do not modify LCD.c.
 * Check that the pin #defines at the top of LCD.h match the wiring above.
 * Useful functions:
 *   LCD_init(), LCD_cursor_set(row, col),
 *   LCD_write_string("..."), LCD_write_variable(value, width)
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"
#include "LCD.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

static void init(void) {
    // TODO: 16 MHz HFINTOSC (OSCCONbits.IRCF / SCS)
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 0b10;
    // TODO: Disable analog on PORTA and PORTD (the LCD pins must be digital)
}

void main(void) {
    init();
    LCD_init();

    int16_t counter = 0;
    int16_t round = 0;

    // TODO: Write the static labels once:
    //   line 1: "Counter:"
    //   line 2: "Round:"

    while (1) {
        // TODO: Each second:
        //   1. Move the cursor after "Counter:" and print counter
        //      (LCD_write_variable with a 2-character width)
        //   2. Move the cursor after "Round:" and print round
        //   3. Delay 1 second
        //   4. Increment counter; when it passes 15, reset it to 0
        //      and increment round
        (void)counter;
        (void)round;
    }
}
