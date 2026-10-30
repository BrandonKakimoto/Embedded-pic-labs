/*
 * File:   lab8.c
 * Author: Brandon Kakimoto & Jason Lopez
 *
 * Lab 8 - DHT11 Humidity and Temperature Sensor
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Reads humidity and temperature from a DHT11 sensor and displays the
 *   values on a 16x2 LCD, using the single-wire DHT11 protocol with
 *   timing-based bit decoding.
 *
 * Pin Assignments:
 *   RD0        - LCD RS
 *   RD2        - LCD EN
 *   RA4-RA7    - LCD D4-D7
 *   RD1        - DHT11 data line (with 10K pull-up to VDD)
 *
 * DHT11 Wiring:
 *   Pin 1 (VDD)  - 5V (or 3.3V)
 *   Pin 2 (DATA) - RD1 + 10K pull-up to VDD
 *   Pin 3 (NC)   - not connected
 *   Pin 4 (GND)  - GND
 *
 * DHT11 Protocol (40 bits total, MSB first):
 *   Byte 1: Humidity integer
 *   Byte 2: Humidity decimal (always 0 for DHT11)
 *   Byte 3: Temperature integer
 *   Byte 4: Temperature decimal (always 0 for DHT11)
 *   Byte 5: Checksum (sum of bytes 1-4, lower 8 bits)
 *
 * LCD driver (LCD.c / LCD.h) is PROVIDED - do not modify LCD.c.
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"
#include "LCD.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

/* DHT11 Pin Definitions */
#define DHT11_TRIS  TRISDbits.TRISD1
#define DHT11_LAT   LATDbits.LATD1
#define DHT11_PORT  PORTDbits.RD1

/* DHT11 Data Storage: humI, humD, tempI, tempD, checksum */
static uint8_t dht11_data[5];

static void init(void) {
    // 16 MHz HFINTOSC
    OSCCONbits.IRCF = 0b111;        // 16 MHz HFINTOSC
    OSCCONbits.SCS = 0b10;          // Use the internal oscillator as the system clock
    
    // ANSELA / ANSELD digital (LCD + DHT11 pins)
    ANSELA = 0x00;      // Sets PORTA to digital
    ANSELD = 0x00;      // Sets PORTD to digital
}

/**
 * Send the start signal and read 40 bits from the DHT11.
 * Returns 1 on success (checksum valid), 0 on failure.
 */
static uint8_t DHT11_read(void) {
    // TODO - implement the DHT11 protocol:
    //
    // Step 1: MCU start signal
    //   - Drive the data line LOW (output mode) for at least 18 ms
    //   - Release: drive HIGH briefly (~30 us), then switch to INPUT
    //     and let the pull-up hold the line HIGH
    //
    // Step 2: Sensor response (use timeouts so a missing sensor
    //         cannot hang the program!)
    //   - Wait out: line HIGH -> LOW  (~80 us response low)
    //   - Wait out: LOW -> HIGH      (~80 us response high)
    //   - Wait out: HIGH -> LOW      (start of first bit)
    //
    // Step 3: Read 40 bits (5 bytes, MSB first). For each bit:
    //   - Wait through the ~50 us LOW bit-start
    //   - Delay ~35 us, then sample the line:
    //       still HIGH -> bit is '1' (then wait for the HIGH to end)
    //       already LOW -> bit is '0'
    //   - Shift the bit into dht11_data[i]
    //
    // Step 4: Verify the checksum:
    //   (bytes 0+1+2+3) & 0xFF must equal byte 4
    //
    // Return 1 only if everything succeeded.
    return 0;
}

void main(void) {
    init();
    LCD_init();

    // TODO: Write static labels:
    //   line 1: "Temp:       C"
    //   line 2: "Hum :       %"

    // DHT11 needs ~1 second to stabilize after power-on
    __delay_ms(1000);

    while (1) {
        // TODO: if DHT11_read() succeeds:
        //   - print dht11_data[2] (temperature integer) on line 1
        //   - print dht11_data[0] (humidity integer) on line 2
        // else show an error indicator on the LCD.
        //
        // The DHT11 requires at least 1 second between readings -
        // delay 2 seconds per loop.
        __delay_ms(2000);
    }
}
