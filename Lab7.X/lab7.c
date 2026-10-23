/*
 * File:   lab7.c
 * Author: Brandon Kakimoto & Jason Lopez
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
    //   If the ADC interrupt flag (PIR1bits.ADIF) is set:
    if (PIR1bits.ADIF) {
        //   1. Read ADRESH:ADRESL into adcResult
        adcResult = ((uint16_t)ADRESH << 8) | ADRESL;       // Merge upper and lower bytes into 10-bit value
        
        //   2. Set adcReady = 1
        adcReady = 1;               // Flag that a fresh ADC conversion is ready
        
        //   3. Clear ADIF
        PIR1bits.ADIF = 0;
        
        //   4. Start the next conversion (GO = 1)
        ADCON0bits.GO = 1;
    }
}

static void init(void) {
    // 16 MHz HFINTOSC
    OSCCONbits.IRCF = 0b111;     // 16MHz HFINTOSC
    OSCCONbits.SCS  = 0b10;      // Use the internal oscillator as the system clock

    // -- Port Configuration --
    // RA0 analog input; PORTB digital outputs (LEDs);
    // RC0/RC1 digital outputs (LEDs); ANSELD = 0 for the LCD pins.
    // (RA4-RA7, RD0, RD2 directions are handled by LCD_init.)
    
    TRISAbits.TRISA0 = 1;          // Sets RA0 to input
    ANSELAbits.ANSA0 = 1;       // Sets RA0 to analog

    ANSELB = 0x00;      // Sets PORTB to digital
    TRISB  = 0x00;       // Sets PORTB to output
    LATB   = 0x00;        // Clears PORTB latches so the LEDs start OFF

    ANSELC = 0x00;
    TRISCbits.TRISC0 = 0;       // Set RC0 to output
    TRISCbits.TRISC1 = 0;       // Set RC1 to output
    LATCbits.LATC0   = 0;         // Clear latch RC0 so bit 8 LED starts OFF
    LATCbits.LATC1   = 0;         // Clear latch RC1 so bit 9 LED starts OFF

    ANSELD = 0x00;              // Set PORTD to digital
    
    // -- ADC Configuration --
    // ADCON0 - select channel AN0, turn the ADC module ON
    ADCON0bits.CHS = 0b00000;       // Select Channel 0 (AN0)
    ADCON0bits.ADON = 1;            // Turns the ADC module ON
    
    // ADCON1 - positive reference = VDD, negative reference = VSS
    ADCON1bits.PVCFG = 0b00;        // Routes VDD to the positive reference
    ADCON1bits.NVCFG = 0b00;        // Routes VSS to the negative reference
    
    // ADCOn2 - right-justified, 8 TAD acquisition, FOSC/32 clock
    ADCON2bits.ADFM = 1;            // Sets ADCON2 to right-justified mode; ADRESH = [0,0,0,0,0,0,b9,b8]   ADRESL = [b7,b6,b5,b4,b3,b2,b1,b0]
    ADCON2bits.ACQT = 0b100;        // 8 TAD acquisition time
    ADCON2bits.ADCS = 0b010;        // FOSC/32 clock

    // -- Interrupt Configuration --
    // clear ADIF, then enable in this order:
    //       ADIE (peripheral source), PEIE (peripheral group), GIE (global)
    PIR1bits.ADIF   = 0;          // Clear the ADIF flag
    PIE1bits.ADIE   = 1;          // Enable ADC interrupt
    INTCONbits.PEIE = 1;          // Enable peripheral interrupts
    INTCONbits.GIE  = 1;          // Enable global interrupts
}

void main(void) {
    init();
    LCD_init();

    // Write static labels: line 1 "ADC:", line 2 "Volt:"
    LCD_cursor_set(1, 1);
    LCD_write_string("ADC:");
    
    LCD_cursor_set(2, 1);
    LCD_write_string("Volt:");
    
    // Start the FIRST conversion here (the ISR keeps it going)
    ADCON0bits.GO = 1;
    
    while (1) {
        if (adcReady) {
            adcReady = 0;       // Reset flag and wait for next conversion

            // 1. Copy adcResult to a local variable
            uint16_t adc = adcResult;
            
            // 2. Lower 8 bits -> LATB; bits 8-9 -> LATC0/LATC1
            LATB = (uint8_t)(adc & 0x00FF);                            // Output lower 8 bits (bits 0-7) to PORTB LEDs
            LATC = (LATC & 0xFC) | ((adc >> 8) & 0x03);                // Output upper 2 bits (bits 8-9) to RC0-RC1
            
            // 3. voltage = (adc / 1023.0) * 5.0
            float voltage = (adc / 1023.0f) * 5.0f;
            
            // 4. LCD line 1: print adc (LCD_write_variable, width 4)
            LCD_cursor_set(1, 6);
            LCD_write_variable(adc, 4);             // Display 10-bit decimal ADC value
            
            // 5. LCD line 2: print voltage (LCD_write_float, width 4, 2 decimals) followed by 'V'
            LCD_cursor_set(2, 7);
            LCD_write_float(voltage, 4, 2);             // Display voltage with 2 decimals
            LCD_write_char('V');                        // Append 'V' unit to the display
                      
            // 6. Delay ~100 ms (about a 10 Hz update rate)
            __delay_ms(100);
            
        }
    }
}
