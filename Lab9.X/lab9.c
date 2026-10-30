/*
 * File:   lab9.c
 * Author: <Jason Lopez>
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
    OSCCONbits.IRCF = 0b111; // 16 MHz HFINTOSC
    OSCCONbits.SCS = 0b10;     
    ANSELA = 0x00;              // ANSELA/B/C/D digital
    ANSELB = 0x00;
    ANSELC = 0x00; 
    ANSELD = 0x00;
            
    TRIG_TRIS = 0;        //TRIG (RB0) is set as output 
    ECHO_TRIS = 1;       //ECHO (RB1) set as input
    ALERT_TRIS = 0;       //Alert LED (RB2) set as Output  
    ALERT_LAT= 0;          //Alert LED (RB2) off 
    TRIG_LAT = 0;         //TRIG (RB0) start Low 
    
    T1CONbits.TMR1CS = 0;       // Timer1 internal clock
    T1CONbits.T1CKPS = 0b11;    // 1:8 Prescaler 
    T1CONbits.TMR1ON = 0;       // Timer1 off for now                     
                                
    TMR1H = 0;                   // TMR1 high and low bits cleared
    TMR1L = 0;       
    PIR1bits.TMR1IF = 0;        // Overflow flag cleared 
}

/**
 * Perform one ultrasonic measurement.
 * Returns distance in centimeters, or 0 if no echo / timeout.
 */
static uint16_t measure_distance(void) {
    TMR1H = 0;              // 1. Clear Timer1 (TMR1H/TMR1L) and TMR1IF
    TMR1L = 0;        
    PIR1bits.TMR1IF = 0; 
    
    TRIG_LAT = 1;   // 10 us trigger pulse on TRIG
    __delay_us(10);
    TRIG_LAT = 0;
    
    unsigned int timeout = 10000; // Wait for ECHO to go high with a timeout that returns 0 if echo never turns high
    
        while (ECHO_PORT == 0){
                timeout -- ;
                if (timeout == 0){
                   return 0;  
            }            
        }

    T1CONbits.TMR1ON = 1;       //Start Timer1
    
    while (ECHO_PORT == 1){      //If TMR1IF overflows while waiting for echo to go low stop TMR1 and return 0
        if ( PIR1bits.TMR1IF == 1){
            T1CONbits.TMR1ON = 0;
           
            return 0;
        }
    }                            
    
    T1CONbits.TMR1ON = 0;// STOP TMR1 
    
    uint16_t ticks; //       Stop Timer1 and read TMR1H:TMR1L
    uint16_t distance_cm;
    ticks = ((uint16_t) TMR1H << 8)| TMR1L; // read TMR1H and TMR1L, shift TMR1H left 8 bits and use TMR1L as lower 8 bits 
    
    distance_cm = (ticks * 2)/58 ;      //Convert ticks to cm:
                                        //each tick = 2 us  ->  distance_cm = ticks * 2 / 58
    return distance_cm;
}

void main(void) {
    init();
    LCD_init();

    LCD_cursor_set (1,1);             //line 1 label distance cm on LCD display
    LCD_write_string ("distance cm"); 

    __delay_ms(500);    /* Let the sensor stabilize */
   
    while (1) {
        uint16_t distance = measure_distance();

    LCD_cursor_set (2,1);                   //clear line 2 of LCD 
    LCD_write_string ("               ");
    LCD_cursor_set (2,1);
    
    if (distance == 0){                         // if distance == 0 LCD display "out of range"
        LCD_write_string ("Out of range");
        ALERT_LAT = 0;
    }                                          
    else if (distance <= THRESHOLD_CM){         //if distance <= threshold_cm LCD display "target detected"
        LCD_write_string ("Target Detected");
        ALERT_LAT = 1;                    // Turn on LED for 1 sec then off
        __delay_ms(1000);
        ALERT_LAT = 0;
       
    }                                        
    
    else{ 
        LCD_write_variable (distance,0);
       LCD_write_string (" cm");           // display distance value and led off
       ALERT_LAT = 0;
        
    }     
        (void)distance;

        __delay_ms(200);    
    }
}
