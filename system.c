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
#include <stdint.h>        /* For uint8_t definition */
#endif
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
    /* TODO Add clock switching code if appropriate.  */

    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
    
//OSCCONbits.IRCF=0b110; //4Mhz
    
//000 = 31kHz
//001 = 125kHz
//010 = 250kHz
//011 = 500kHz
//100 = 1MHz
//101 = 2MHz
//110 = 4MHz (default)
//111 = 8MHz
    
OSCCONbits.IRCF=0b110; //4Mhz
OSCCONbits.OSTS=1; //Oscillator Start-up Time-out Status bit. 1 = Device is running from the clock defined by FOSC<2:0> of the CONFIG register
//OSCCONbits.HTS=1; // HFINTOSC Status bit, 1 = HFINTOSC is stable
//OSCCONbits.LTS=0; // LFINTOSC Stable bit, 1 = LFINTOSC is stable
OSCCONbits.SCS=0; // System Clock Select bit. 0 = Clock source defined by FOSC<2:0> of the CONFIG register
OSCTUNEbits.TUN=19; // calibrate internal clock, 1sec timer0 = 19-1004 ms, 20-996 ms
//Wait stabilization
while (OSCCONbits.HTS==0);

    
        
    
}
