 /* File:   lab4.c
 * Author: <Jason Lopez, Brandon Kakimoto>
 *
 * Lab 4 - Traffic Light Controller
 *         (FSM + Safety Mode + Pedestrian Crossing)
 * ECE 3301L - Introduction to Microcontrollers Laboratory
 *
 * Description:
 *   Formal FSM-based traffic light controller with:
 *   - All-Red flashing safety mode (on-board button S2 = RC5)
 *   - Pedestrian crossing with 9-to-0 countdown on a 7-segment display
 *     (external SPST button on RE0)
 *   The pedestrian request is LATCHED and serviced after the EW Yellow
 *   phase. During the countdown, NS has Green and EW has Red.
 *
 * Pin Assignments:
 *   PORTD - South (RD5=G, RD4=Y, RD3=R) and North (RD2=G, RD1=Y, RD0=R)
 *   PORTB - East  (RB5=G, RB4=Y, RB3=R) and West  (RB2=G, RB1=Y, RB0=R)
 *           RB6/RB7 preserved (PGC/PGD for LVP programming)
 *   PORTC - 7-Segment Display (common anode, LOW = ON)
 *           RC0=a, RC1=b, RC2=c, RC3=d, RC4=e, RC6=f, RC7=g
 *           RC5 = On-board button S2 (safety, active-low)
 *   PORTE - RE0 = External pedestrian button (active-low, 10k pull-up)
 *   PORTA - RA4 = On-board LED (pedestrian request indicator)
 *
 * FSM States:
 *   EW_GREEN      - EW Green,  NS Red         (6 s)   -> EW_YELLOW
 *   EW_YELLOW     - EW Yellow, NS Red         (3 s)   -> PED_COUNTDOWN or NS_GREEN
 *   PED_COUNTDOWN - NS Green,  EW Red + 7-seg (10 s)  -> NS_GREEN
 *   NS_GREEN      - NS Green,  EW Red         (6 s)   -> NS_YELLOW
 *   NS_YELLOW     - NS Yellow, EW Red         (3 s)   -> EW_GREEN
 *   ALL_RED_FLASH - All Reds flashing (safety)        -> EW_GREEN on S2 press
 */

#include <xc.h>
#include <stdint.h>
#include "Configuration.h"

#define _XTAL_FREQ 16000000UL

/* FSM State Definitions */
typedef enum {
    EW_GREEN,
    EW_YELLOW,
    NS_GREEN,
    NS_YELLOW,
    PED_COUNTDOWN,
    ALL_RED_FLASH
} TrafficState;

/* 7-Segment Patterns for PORTC (Common Anode: 0=ON, 1=OFF)
 *
 *   Bit:   7  6  5  4  3  2  1  0
 *   Seg:   g  f  -  e  d  c  b  a     (bit 5 = RC5 button, always mask it)
 *
 *        aaa
 *       f   b
 *        ggg
 *       e   c
 *        ddd
 */
static const uint8_t sevenSeg[10] = {
    0xA0, // TODO: 0 (a,b,c,d,e,f ON, g OFF)
    0xF9, // TODO: 1
    0x64, // TODO: 2
    0x70, // TODO: 3
    0x39, // TODO: 4
    0x32, // TODO: 5
    0x22, // TODO: 6
    0xF8, // TODO: 7
    0x20, // TODO: 8
    0x30  // TODO: 9
};

#define SEG_BLANK 0xDF          /* All segments OFF (preserves RC5 bit) */

/* Pedestrian request flag (latched) */
static uint8_t pedRequest = 0;

static void init(void) {
    
    OSCCONbits.IRCF = 0b111; //set Oscillator 16 MHz
    OSCCONbits.SCS = 0b10;
    
    ANSELA = 0x00;
    ANSELB = 0x00;  //make PORTA/B/C/D/E digital
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
   
    
    TRISD &= 0xC0; // PORTD RD0-RD5 outputs, PORTB RB0-RB5 outputs 
    TRISB &= 0xC0;
    TRISC &= 0x00; // PORTC segments as outputs, RC5 as input (button S2)
    TRISC = 0x20;  

    TRISEbits.TRISE0 = 1;// RE0 input (pedestrian button), RA4 output (request led)
    TRISAbits.TRISA4 = 0;
    
    

    LATC = (LATC & 0x20)| SEG_BLANK ;  // Blank the 7-segment display
}

/** Update traffic lights, touching only bits 0-5 of each port. */
static void set_lights(uint8_t rd_val, uint8_t rb_val) {
    
    LATD = (LATD & 0xC0)| (rd_val & 0x3F);
    LATB = (LATB & 0xC0)| (rb_val & 0x3F);

}

/** Show one digit (0-9) on the 7-segment display, preserving RC5. */
static void seg_show(uint8_t digit) {
    if(digit<=9){
        LATC=(LATC &0x00)|(sevenSeg[digit]&(uint8_t)~0x00);
    }
}

/**
 * Delay in ~10 ms slices while polling the buttons, so a press is never
 * missed during a long state delay.
 * Returns early if the safety button (S2) is pressed.
 */
static uint8_t delay_and_poll(uint16_t ms) {
    uint16_t i;
    
    for (i=0; i < ms; i+=10) {
        if (PORTEbits.RE0 == 0) {
       
       pedRequest = 1;
       LATAbits.LATA4 = 0;
       
         }  
    if (PORTCbits. RC5 == 0){
        
        return 1; 
        
         }
        
        __delay_ms(10);
        
    }
   
    return 0;
}

void main(void) {
    init();

    TrafficState state = EW_GREEN;

    while (1) {
       uint8_t S2PRESSED;
       switch (state) {
           
           case EW_GREEN :// EW_GREEN:lights, delay_and_poll(6000), -> EW_YELLOW
               set_lights(0x09, 0x24);
               
            
               
               if (delay_and_poll(6000)) {
                   state = ALL_RED_FLASH;
               }
               else {
                   state = EW_YELLOW;
               }
               break; 
               
           case EW_YELLOW : // EW_YELLOW:lights, delay_and_poll(3000),PED_COUNTDOWN: NS green / EW red; count 9..0 on the 7-seg,
                            // 1 second per digit; clear pedRequest and RA4; blank display; -> NS_GREEN
               set_lights(0x09, 0x12);
               
               S2PRESSED = delay_and_poll(3000); 
               
               if (S2PRESSED){
                   state = ALL_RED_FLASH;
               }
               else if (pedRequest){
                   state = PED_COUNTDOWN; 
               }
               else {
                   state = NS_GREEN;
               }
               
               break; 
               
           case PED_COUNTDOWN : {  
               set_lights(0x24, 0x09);
               
               S2PRESSED = 0;
            uint8_t count = 10;
            
            while(count > 0){
                count--; 
              seg_show(count);
              __delay_ms(1000);
            } 
            
        pedRequest=0;  //clear the ped request 
        
        LATAbits.LATA4 = 1;// turn off the led

        LATC = (LATC & 0x20) | SEG_BLANK;// blank the display 
        
        state = NS_GREEN;

    break;
    
       }   
           case NS_GREEN:  //   NS_GREEN: lights, delay_and_poll(6000), -> NS_YELLOW
               set_lights(0x24, 0x09);
               
               if (delay_and_poll(6000)){
                   state = ALL_RED_FLASH;
               }
               else {
                   state = NS_YELLOW;
               }
               
    break;
               
           case NS_YELLOW:
               set_lights(0x12, 0x09);
               
               S2PRESSED= delay_and_poll(3000);
               
               if (S2PRESSED){
                   state = ALL_RED_FLASH;
                  
               }
               else {
                   state = EW_GREEN;
               }
               
               break; 
               
           case ALL_RED_FLASH:
            while (PORTCbits.RC5==1){  
            }
            
            while(PORTCbits.RC5 == 0){
                set_lights(0x09,0x09);
                __delay_ms(500);
                
                set_lights(0x00, 0x00);
                __delay_ms(500);
                
               
            }
            
            state= EW_GREEN;
                    
    break;
           
             
       }
       
       
        (void)state;
    }
}
