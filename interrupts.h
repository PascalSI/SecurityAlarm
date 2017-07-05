
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include <xc.h> // include processor files - each processor file is guarded.  
#ifndef __stdint_are_defined
#define __stdint_are_defined
#include <stdint.h>        /* For uint8_t definition */
#endif


#define TMR0InitValue 6;    

volatile int32_t t0_millis = 0;     /**< Milli-Second Counter.*/
//#ifdef usePC2Keyboard 
volatile uint8_t pc2kbd_answer=0x0, pc2kbd_count=0;
bit pc2kbd_ready;
bit pc2kbd_parity;
bit pc2kbd_start;
bit pc2kbd_data;
int32_t pc2kbd_timer;


//#ifdef useVoltageDetector
volatile bit lowVoltageIs;
//#endif

//#endif
void interrupt isr(void);


#endif	/* XC_HEADER_TEMPLATE_H */

