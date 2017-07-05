/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#ifndef __stdint_are_defined
#define __stdint_are_defined
#include <stdint.h>         /* For uint8_t definition */
#endif
#include <stdbool.h>        /* For true/false definition */

#include "interrupts.h"
#include "pc2keyboard.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's 
 * are baseline devices.  Unfortunately the baseline detection macro is 
 * _PIC12 */
#ifndef _PIC12

/**
 * @brief Interrupt Service Routine (High).
 *
 * Interrupt Service Routine for high priority interrupts.
 * 
 * @note Call to this function is done automatically whenever the high priority
 * interrupt occurs and its corresponding flag gets set.
 */

void interrupt isr(void) {
    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */

#if 1


    /* TODO Add interrupt routine code here. */

    /* Determine which flag generated the interrupt */
    //    if(<Interrupt Flag 1>)
    //    {
    //        <Interrupt Flag 1=0>; /* Clear Interrupt Flag 1 */
    //    }
    //    else if (<Interrupt Flag 2>)
    //    {
    //        <Interrupt Flag 2=0>; /* Clear Interrupt Flag 2 */
    //    }
    //    else
    //    {
    //        /* Unhandled interrupts */
    //    }

    // Timer0 Interrupt - Freq = 1000.00 Hz - Period = 0.001000 seconds
    if (INTCONbits.T0IF == 1) // timer 0 interrupt flag
    {
        INTCONbits.T0IF = 0; // clear the flag
        //INTCONbits.T0IE = 1; // reenable the interrupt
        TMR0 = TMR0InitValue; // reset the timer preset count
        t0_millis++;
    }
#ifdef usePC2Keyboard    
    else if (INTCONbits.INTF == 1) {
        INTCONbits.INTF = 0;
        pc2kbd_data = PC2Keyboard_DATA; //ASAP rember vlaue of data bit .
        pc2kbd_count++;
        if (pc2kbd_count == 1) {
            if ((t0_millis - pc2kbd_timer) > 4) {
                //wait real start bit after previos packet of data max ~80ms. 
                //next package of data when repated key is ~86ms
                //I use 4ms
                pc2kbd_start = (pc2kbd_data == 0); //stat bit always must be 0
                pc2kbd_timer = t0_millis; //remember time of last 1 bit
                if (pc2kbd_start) {
                    pc2kbd_answer = 0; // initial value for scaned code
                    pc2kbd_parity = 1; // inital value for odd parity 
                } else {
                    //start bit is wrong, reset counter
                    pc2kbd_count = 0;
                }
            } else {
                //time of start bit is wrong, reset counter
                pc2kbd_count = 0;
            }
        } else if (pc2kbd_count > 1 && pc2kbd_count < 10) {
            if (pc2kbd_data == 1) {
                // data bit is high
                pc2kbd_answer = pc2kbd_answer | 0b10000000; // if input bit is high   
                pc2kbd_parity ^= 1; // calc odd parity by XOR with 1
            }
            if (pc2kbd_count < 9) {
                pc2kbd_answer = pc2kbd_answer >> 1; // need to shift 7times,not to loose last bit
            }
        } else if (pc2kbd_count == 10) {
            //check claculated odd parity with real 10 bit 
            pc2kbd_parity = (pc2kbd_parity == pc2kbd_data);
        } else if (pc2kbd_count == 11) {
            //ready check if start bit state, parity state and real stop bit data is set
            pc2kbd_ready = pc2kbd_start && pc2kbd_parity && pc2kbd_data;
            if (!pc2kbd_ready) {
                pc2kbd_answer = 0;
            }
            //reset counter
            pc2kbd_count = 0;
        }
    }
#endif    
#ifdef useVoltageDetector
    else if (PIR1bits.ADIF) { //PIR1bits.ADIF
#define adc_val ADRESL             
        PIR1bits.ADIF = 0;
        ADCON0bits.ADON = 0; // Turn on/off ADC module
        PIE1bits.ADIE = 0;
        lowVoltageIs = (adc_val > 120u);
    }
#endif

#endif

}
#endif


