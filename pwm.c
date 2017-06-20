/* 
 * File:   pwm.c
 * Author: xpress_embedo
 *
 * Created on June 3, 2017, 9:01 PM
 */

//#include "config.h"
#include "pwm.h"



/*
 * Important Formulas
 * PWM Period = [(PR2)+1]*4*TOSC*(TMR2 Prescale Value)
 * TOSC = 1/FOSC
 * PWM Period = [(PR2)+1]*4*(TMR2 Prescale Value)/FOSC
 * PWM Frequency = FOSC/([(PR2)+1]*4*(TMR2 Prescale Value))
 * PR2+1 = FOSC/(PWM Frequency*4*TM2 Prescale Value)
 * PR2 = FOSC/(PWM Frequency*4*TM2 Prescale Value) - 1
 * 
 */


static uint8_t timer_prescale = 1u;
static uint32_t pwm_freq = 0u;

uint8_t PWM1_Init(const uint32_t frequency)
{
  uint8_t result = 1;
  uint32_t temp = 0u;
  uint32_t temp2 = 0u;
  
  /* Steps */
  /* 1) Disable the CCPx pin output driver by setting the associated TRIS bit*/
  TRISCbits.TRISC5 = TRIS_OUTPUT;
  /* 2) Load the PR2 register for Timer2 with PWM Period Value.*/
  pwm_freq = frequency;
  temp = _XTAL_FREQ/frequency;
  temp /= 4u;
  temp /= timer_prescale;
  if ( temp < 255u )
    PR2 = (uint8_t)temp - 1;
  else
  {
    timer_prescale = 4u;
    temp2 = temp/timer_prescale;
    if ( temp2 < 255u )
    {
      PR2 = (uint8_t)temp2 - 1;
    }
    else
    {
      timer_prescale = 16u;
      temp = temp/timer_prescale;
      if ( temp < 255u )
      {
        PR2 = (uint8_t)temp - 1;
      }
      else
      {
        result = 0;
      }
    }
  }
  
  /* 3) Configure the CCP Module for the PWM Mode by loading the CCPxCON with
   * the appropriate values
   */
  // This will be done using the PWM Start Function
  CCPR1L = 0x00;
  CCP1CON = 0x00;  //(Single Output RC5, P1A)
  
  return result;
}

/*
 * Pulse Width = (CCPRxL:CCPxCON<5:4>*TOSC*(TMR2 Prescale Value)
 * 
 * Duty Cycle Ratio = Pulse Width/ PWM Period
 * Duty Cycle Ratio = (CCPRxL:CCPxCON<5:4>*TOSC*(TMR2 Prescale Value)
 *                    ------------------------------------------------
 *                        [(PR2)+1]*4*TOSC*(TMR2 Prescale Value)
 * Duty Cycle Ratio = (CCPRxL:CCPxCON<5:4>/([(PR2)+1]*4)
 */

void PWM1_Set_Duty(uint8_t duty_ratio)
{
  uint16_t value = duty_ratio;
  value = value * (PR2+1) * 4u;
  value = value/100u;
  CCPR1L = (value>>2u);
  value = value & 0x03;
  CCP1CON &= 0xCF;
  CCP1CON |= value<<4;
  switch(timer_prescale)
  {
    case 1:
      T2CON = 0b00000100;
      break;
    case 4:
      T2CON = 0b00000101;
      break;
    case 16:
      T2CON = 0b00000111;
      break;
  }
}

void PWM1_Start()
{
  TRISC2 = 0;
  CCP1CON |= 0x0C;
}

void PWM1_Stop()
{
  CCP1CON &= 0xF3;
}