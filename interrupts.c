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

void interrupt isr(void)
{
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
        INTCONbits.T0IE = 1; // reenable the interrupt
        TMR0 = TMR0InitValue; // reset the timer preset count
        t0_millis++;
    }

#endif

}
#endif


