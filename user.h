/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */


#include "interrupts.h"
#include "system.h"

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif


#ifndef USER_H
#define	USER_H
#ifndef __stdint_are_defined
#define __stdint_are_defined
#include <stdint.h>        /* For uint8_t definition */
#endif
#include <stdbool.h>       /* For true/false definition */

//#define useInverseInputSensors


volatile union {
    unsigned char byte;
    PORTAbits_t bits;
} LATPORTA;

#define LATPORTA_FLUSH PORTA = LATPORTA.byte

volatile union {
    unsigned char byte;
    PORTBbits_t bits;
} LATPORTB;

#define LATPORTB_FLUSH PORTB = LATPORTB.byte

volatile union {
    unsigned char byte;
    PORTCbits_t bits;
} LATPORTC;

#define LATPORTC_FLUSH PORTC = LATPORTC.byte

#define TRIS_OUTPUT 0
#define TRIS_INPUT 1
#define ANS_ANALOG 1
#define ANS_DIGITAL 0

// LEDS
#define LED_SYS1 LATPORTC.bits.RC2
#define LED_SYS1_ON 1
#define LED_SYS1_OFF !LED_SYS1_ON
#define LED_SYS1_TRIS TRISCbits.TRISC2

#define LED_DOOR LATPORTC.bits.RC5
#define LED_DOOR_ON 0
#define LED_DOOR_OFF !LED_DOOR_ON
#define LED_DOOR_TRIS TRISCbits.TRISC5
#define LEDS_FLUSH_PORT LATPORTC_FLUSH

// BUZZER
#define BUZZER LATPORTC.bits.RC4
#define BUZZER_ON 1
#define BUZZER_OFF !BUZZER_ON
#define BUZZER_TRIS TRISCbits.TRISC4
#define BUZZER_FLUSH_PORT LATPORTC_FLUSH


// KEYPAD
// KEYPAD_ROWS
//ROW1
#define KEYPAD_ROW1 LATPORTC.bits.RC0
#define KEYPAD_ROW1_ON 0U
#define KEYPAD_ROW1_OFF !KEYPAD_ROW1_ON
#define KEYPAD_ROW1_TRIS TRISCbits.TRISC0
//ROW2
#define KEYPAD_ROW2 LATPORTC.bits.RC1
#define KEYPAD_ROW2_ON 0U
#define KEYPAD_ROW2_OFF !KEYPAD_ROW2_ON
#define KEYPAD_ROW2_TRIS TRISCbits.TRISC1
//ROW3
#define KEYPAD_ROW3 LATPORTC.bits.RC2
#define KEYPAD_ROW3_ON 0U
#define KEYPAD_ROW3_OFF !KEYPAD_ROW3_ON
#define KEYPAD_ROW3_TRIS TRISCbits.TRISC2
//KEYPAD COLS
//COL1
#define KEYPAD_COL1 PORTAbits.RA0
#define KEYPAD_COL1_ON 0U
#define KEYPAD_COL1_OFF !KEYPAD_COL1_ON
#define KEYPAD_COL1_TRIS TRISAbits.TRISA0
//COL2
#define KEYPAD_COL2 PORTAbits.RA1
#define KEYPAD_COL2_ON 0U
#define KEYPAD_COL2_OFF !KEYPAD_COL2_ON
#define KEYPAD_COL2_TRIS TRISAbits.TRISA1
//COL3
#define KEYPAD_COL3 PORTAbits.RA2
#define KEYPAD_COL3_ON 0U
#define KEYPAD_COL3_OFF !KEYPAD_COL3_ON
#define KEYPAD_COL3_TRIS TRISAbits.TRISA2

#define KEYPAD_ROWS_FLUSH_PORT LATPORTC_FLUSH

//DOOR SENSORS

#define DOOR_BUTTON PORTCbits.RC6
#define DOOR_BUTTON_ON 0
#define DOOR_BUTTON_OFF !DOOR_BUTTON_ON
#define DOOR_BUTTON_TRIS TRISCbits.TRISC6

#define LOCK1 PORTAbits.RA4

#ifdef useInverseInputSensors
#define LOCK1_ON 1
#else
#define LOCK1_ON 0
#endif

#define LOCK1_OFF !LOCK1_ON
#define LOCK1_TRIS TRISAbits.TRISA4

#define LOCK2 PORTAbits.RA5

#ifdef useInverseInputSensors
#define LOCK2_ON 1
#else
#define LOCK2_ON 0
#endif
#define LOCK2_OFF !LOCK2_ON
#define LOCK2_TRIS TRISAbits.TRISA5

#define DOOR PORTCbits.RC3
#ifdef useInverseInputSensors
#define DOOR_ON 1
#else
#define DOOR_ON 0
#endif
#define DOOR_OFF !DOOR_ON
#define DOOR_TRIS TRISCbits.TRISC3


void Timer0_Init(void);
int16_t millis(void);


void InitApp(void);         /* I/O and Peripheral Initialization */

char UART_Init(const long int baudrate);
void UART_Write(char data);
void UART_Write2byte(int16_t data);
char UART_TX_Empty();
void UART_Write_Text(char *text);
char UART_Data_Ready();
char UART_Read();
void UART_Read_Text(char *Output, uint8_t length);


#endif