/* 
 * File:   pc2keyboard.h
 * Author: lex
 *
 * Created on 2 ????? 2017, 18:49
 * 
 * Used 2 pins
 * PC2 CLOCK - KEYPAD_COL3 (INT)
 * PC2 DATA  - KEYPAD_COL1
 * 
 */

#ifndef PC2KEYBOARD_H
#define	PC2KEYBOARD_H

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include "user.h"
#include "interrupts.h"



#define PC2Keyboard_CLOCK  KEYPAD_COL3
#define PC2Keyboard_DATA   KEYPAD_COL1



void PC2Keyboard_Init(void);
void PC2Keboard_Process() ;

#endif	/* PC2KEYBOARD_H */

