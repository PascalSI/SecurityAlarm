/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include "user.h"
#include "interrupts.h"

//#include "drv_uart1.h"
//#include "drv_uart2.h"
//#include "drv_uart3.h"
//#include "drv_uart4.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    /* TODO Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Enable interrupts */
    
        //ANALOG PORT SET TO DIGITAL
    ANSEL = 0b0;
    ANSELH = 0b0;

    
    
    LED_SYS1_TRIS = TRIS_OUTPUT;
    LED_DOOR_TRIS = TRIS_OUTPUT;
    LED_SYS1 = LED_SYS1_OFF;
    LED_DOOR = LED_DOOR_OFF;
    LEDS_FLUSH_PORT;

    BUZZER_TRIS = TRIS_OUTPUT;
    BUZZER = BUZZER_OFF;
    BUZZER_FLUSH_PORT;

    //KEYPAD
    KEYPAD_ROW1_TRIS = TRIS_OUTPUT;
    KEYPAD_ROW2_TRIS = TRIS_OUTPUT;
    KEYPAD_ROW3_TRIS = TRIS_OUTPUT;
    KEYPAD_COL1_TRIS = TRIS_INPUT;
    KEYPAD_COL2_TRIS = TRIS_INPUT;
    KEYPAD_COL3_TRIS = TRIS_INPUT;

        //WPUA PORTA all WEAK PULL-UPS ON
    OPTION_REGbits.nRABPU=0;
    WPUA = 0b11111111;
    
    
    //SENSORS OF DOOR
    DOOR_BUTTON_TRIS = TRIS_INPUT;
    LOCK1_TRIS = TRIS_INPUT;
    LOCK2_TRIS = TRIS_INPUT;
    DOOR_TRIS = TRIS_INPUT;





    Timer0_Init();

    // Interrupt Registers
    INTCON = 0; // clear the interrpt control register
    INTCONbits.T0IE = 1; // bit5 TMR0 Overflow Interrupt Enable bit...1 = Enables the TMR0 interrupt
    INTCONbits.T0IF = 0; // bit2 clear timer 0 interrupt flag
    INTCONbits.GIE = 1; // bit7 global interrupt enable


}


void Timer0_Init(void){
    //Timer0 Registers Prescaler= 4 - TMR0 Preset = 6 - Freq = 1000.00 Hz - Period = 0.001000 seconds
    OPTION_REGbits.T0CS = 0; // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0; // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0; // bit 3   Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS  = 0b001; // bits 2-0  PS2:PS0: Prescaler Rate Select bits (1:4)
    
//000 1 : 2
//001 1 : 4
//010 1 : 8
//011 1 : 16
//100 1 : 32
//101 1 : 64
//110 1 : 128
//111 1 : 256
    TMR0 = TMR0InitValue; // preset for timer register
}


char UART_Init(const long int baudrate)
{
  unsigned int x;
  x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
  if(x>255)                                       //If High Baud Rage Required
  {
    x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
    BRGH = 1;                                     //Setting High Baud Rate
  }
  if(x<256)
  {
    //  TXSTAbits.
    SPBRG = x;                                    //Writing SPBRG Register
    TXSTAbits.SYNC = 0;                           //Setting Asynchronous Mode, ie UART
    RCSTAbits.SPEN = 1;                           //Enables Serial Port
    TRISBbits.TRISB7 = 1;                         //As Prescribed in Datasheet
    TRISBbits.TRISB6 = 1;                         //As Prescribed in Datasheet
    RCSTAbits.CREN = 1;                           //Enables Continuous Reception
    TXSTAbits.TXEN = 1;                           //Enables Transmission
    return 1;                                     //Returns 1 to indicate Successful Completion
  }
  return 0;                                       //Returns 0 to indicate UART initialization failed


}

void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}

void UART_Write2byte(int16_t data){
  UART_Write((data)>>8);
  UART_Write((data)); 
}

char UART_TX_Empty()
{
  return TRMT;
}

void UART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
    UART_Write(text[i]);
}

char UART_Data_Ready()
{
  return RCIF;
}

char UART_Read()
{
  while(!RCIF);
  return RCREG;
}

void UART_Read_Text(char *Output, uint8_t length)
{
  for(uint8_t i=0;i<length;i++)
  Output[i] = UART_Read();
}

/**
 * @brief Millis.
 *
 * Returns the number of milliseconds since the board began running the current 
 * program. This number will overflow (go back to zero), after approximately 
 * 50 days.This function returns time in milli-second
 * @return Number of milliseconds since the program started (#u32_t)
 */
int16_t millis(void) {
    return t0_millis;
}




