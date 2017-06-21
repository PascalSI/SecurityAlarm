
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

volatile int16_t t0_millis = 0;     /**< Milli-Second Counter.*/

#endif	/* XC_HEADER_TEMPLATE_H */

