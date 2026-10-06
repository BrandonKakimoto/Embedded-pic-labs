/*
 * File:   lab9.c
 * Author: <your names here>
 *
 * Lab 9 - Ultrasonic Distance Measurement and Target Detection
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Measures distance using an HC-SR04 ultrasonic sensor.
 *   Displays distance in cm on a 16x2 LCD.
 *   Activates an LED on RC0 when a target is <= 50 cm away.
 *   Uses Timer1 with a 1:8 prescaler to measure the echo pulse width.
 *
 * Pin Assignments:
 *   RB0        - HC-SR04 TRIG (output)
 *   RB1        - HC-SR04 ECHO (input)
 *   RC0        - Alert LED (output, through 330 ohm resistor)
 *   RD2        - LCD RS
 *   RD3        - LCD EN
 *   RD4-RD7    - LCD D4-D7
 *   RA0        - Contrast pot (VEE on LCD)
 *
 * Timer1 Configuration (16 MHz FOSC):
 *   Clock source: FOSC/4 = 4 MHz
 *   Prescaler: 1:8 -> 500 kHz -> 2 us per tick
 *   Distance formula: distance_cm = (timer_count * 2) / 58
 *     (speed of sound ~34300 cm/s, divide by 2 for the round trip)
 *
 * NOTE: This lab moves the LCD to different pins (RS=RD2, EN=RD3,
 *       D4-D7=RD4-RD7). Update the #defines at the top of LCD.h!
 *       Do not modify LCD.c.
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"
#include "LCD.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

#define THRESHOLD_CM 50         /* Target detection threshold */

/* Pin Definitions */
#define TRIG_TRIS   TRISBbits.TRISB0
#define TRIG_LAT    LATBbits.LATB0
#define ECHO_TRIS   TRISBbits.TRISB1
#define ECHO_PORT   PORTBbits.RB1

#define ALERT_TRIS  TRISCbits.TRISC0
#define ALERT_LAT   LATCbits.LATC0

static void init(void) {
    // TODO: 16 MHz HFINTOSC
    // TODO: ANSELA/B/C/D digital
    // TODO: TRIG output (start LOW), ECHO input, alert LED output (off)

    // TODO: Timer1 - internal clock (FOSC/4), 1:8 prescaler, OFF for now,
    //       TMR1H/TMR1L cleared, overflow flag (TMR1IF) cleared
}

/**
 * Perform one ultrasonic measurement.
 * Returns distance in centimeters, or 0 if no echo / timeout.
 */
static uint16_t measure_distance(void) {
    // TODO: 1. Clear Timer1 (TMR1H/TMR1L) and TMR1IF
    //       2. Send a 10 us trigger pulse on TRIG
    //       3. Wait for ECHO to go HIGH - with a timeout that
    //          returns 0 if the echo never arrives
    //       4. Start Timer1
    //       5. Wait for ECHO to go LOW - if TMR1IF overflows first
    //          (~131 ms), stop the timer and return 0
    //       6. Stop Timer1 and read TMR1H:TMR1L
    //       7. Convert ticks to cm:
    //          each tick = 2 us  ->  distance_cm = ticks * 2 / 58
    return 0;
}

void main(void) {
    init();
    LCD_init();

    // TODO: line 1 static label: "Distance cm"

    __delay_ms(500);    /* Let the sensor stabilize */

    while (1) {
        uint16_t distance = measure_distance();

        // TODO: Clear LCD line 2, then:
        //   distance == 0            -> "Out of range"
        //   distance <= THRESHOLD_CM -> "Target Detected",
        //                               LED on for 1 second, then off
        //   otherwise                -> print the distance value, LED off
        (void)distance;

        __delay_ms(200);    /* Pause before the next measurement */
    }
}
