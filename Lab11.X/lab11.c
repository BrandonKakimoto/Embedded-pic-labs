/*
 * File:   lab11.c
 * Author: <your names here>
 *
 * Lab 11 - Serial Communication Using I2C
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Reads temperature from a TC74A0 via I2C, stores it in a 24LC256
 *   EEPROM, reads it back, and displays everything on a 16x2 LCD.
 *
 * I2C Device Addresses (7-bit):
 *   TC74A0:   0x48 -> Write: 0x90, Read: 0x91
 *   24LC256:  0x50 -> Write: 0xA0, Read: 0xA1
 *
 * Pin Assignments:
 *   RC3        - SCL (I2C clock)
 *   RC4        - SDA (I2C data)
 *   RD0        - LCD RS
 *   RD2        - LCD EN
 *   RA4-RA7    - LCD D4-D7
 *
 * I2C Bus Wiring:
 *   RC3 (SCL) + 4.7K pull-up to VDD
 *   RC4 (SDA) + 4.7K pull-up to VDD
 *   TC74A0: VDD=5V, GND=GND, SDA=RC4, SCL=RC3
 *   24LC256: VDD=5V, VSS=GND, SDA=RC4, SCL=RC3
 *            A0=A1=A2=GND (address 0x50), WP=GND
 *
 * LCD driver (LCD.c / LCD.h) is PROVIDED - do not modify LCD.c.
 */

#include <xc.h>
#include <stdint.h>
#include "PIC18F46K22-Config.h"
#include "LCD.h"

#define _XTAL_FREQ 16000000UL   // 16 MHz HFINTOSC

/* I2C Device Addresses */
#define TC74_WRITE    0x90      /* TC74A0 write address (0x48 << 1) */
#define TC74_READ     0x91      /* TC74A0 read address */
#define TC74_TEMP_REG 0x00      /* Temperature register */

#define EEPROM_WRITE  0xA0      /* 24LC256 write address (0x50 << 1) */
#define EEPROM_READ   0xA1      /* 24LC256 read address */

/* ------------------- I2C Master Functions ------------------- */

/** Wait for any in-progress MSSP operation to complete. */
static void I2C_wait(void) {
    // TODO: spin while (SSPCON2 & 0x1F) or SSPSTATbits.R_nW is set
}

/** Send an I2C Start condition. */
static void I2C_start(void) {
    // TODO: I2C_wait(), set SEN, wait for SEN to clear
}

/** Send an I2C Repeated Start condition. */
static void I2C_restart(void) {
    // TODO: like start, but with RSEN
}

/** Send an I2C Stop condition. */
static void I2C_stop(void) {
    // TODO: like start, but with PEN
}

/**
 * Write one byte on the I2C bus.
 * Returns 0 if the slave ACKed, 1 if NACK.
 */
static uint8_t I2C_write(uint8_t data) {
    // TODO: I2C_wait(), load SSPBUF, wait for BF to clear,
    //       I2C_wait(), return ACKSTAT
    (void)data;
    return 1;
}

/**
 * Read one byte from the I2C bus.
 * ack=1: send ACK (more bytes to read); ack=0: send NACK (last byte).
 */
static uint8_t I2C_read(uint8_t ack) {
    // TODO: enable RCEN, wait for BF, read SSPBUF,
    //       then send ACK or NACK via ACKDT/ACKEN
    (void)ack;
    return 0;
}

/* ------------------- TC74A0 Sensor Functions ------------------- */

/**
 * Read the temperature from the TC74A0.
 * Returns signed temperature in degrees Celsius.
 *
 * Transaction: START, addr+W, register 0x00, REPEATED START,
 *              addr+R, read one byte with NACK, STOP.
 */
static int8_t TC74_read_temp(void) {
    // TODO
    return 0;
}

/* ------------------- 24LC256 EEPROM Functions ------------------- */

/**
 * Write one byte to the EEPROM at the given 16-bit address.
 *
 * Transaction: START, addr+W, address high byte, address low byte,
 *              data byte, STOP - then wait ~10 ms for the internal
 *              write cycle to finish.
 */
static void EEPROM_write_byte(uint16_t addr, uint8_t data) {
    // TODO
    (void)addr;
    (void)data;
}

/**
 * Read one byte from the EEPROM at the given 16-bit address.
 *
 * Transaction: START, addr+W, address high, address low,
 *              REPEATED START, addr+R, read one byte with NACK, STOP.
 */
static uint8_t EEPROM_read_byte(uint16_t addr) {
    // TODO
    (void)addr;
    return 0;
}

/* ------------------- Initialization ------------------- */

static void init(void) {
    // TODO: 16 MHz HFINTOSC
    // TODO: ANSELA/C/D digital

    // -- I2C Master Configuration (MSSP1) --
    // TODO: RC3 and RC4 must both be configured as INPUTS for I2C
    // TODO: SSPCON1 - I2C Master mode (SSPM = 0b1000), enable MSSP (SSPEN)
    // TODO: SSPADD for 100 kHz:
    //       SSPADD = FOSC / (4 * Fscl) - 1   (work it out for 16 MHz)
    // TODO: SSPSTATbits.SMP = 1 (slew rate control off for 100 kHz)
}

/* ------------------- Main Program ------------------- */

void main(void) {
    init();
    LCD_init();

    __delay_ms(500);    /* Let the I2C bus stabilize */

    uint16_t eeprom_addr = 0x0000;

    while (1) {
        // TODO - each cycle:
        //   Stage 1: read the TC74 temperature and show it on the LCD
        //   Stage 2: write it to the EEPROM at eeprom_addr
        //   Stage 3: read it back from the EEPROM
        //   Stage 4: show both values and indicate whether they match
        //   Then increment eeprom_addr (wrap at 0x7FFF) and delay ~3 s.
        (void)eeprom_addr;
    }
}
