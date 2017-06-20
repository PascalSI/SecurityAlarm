/* 
 * File:   pwm.h
 * Author: xpress_embedo
 *
 * Created on June 3, 2017, 9:01 PM
 */

#ifndef PWM_H
#define	PWM_H

//#include "config.h"
#include "system.h"
#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include "user.h"


#ifdef	__cplusplus
extern "C"
{
#endif

 /* Function Prototypes*/
uint8_t PWM1_Init(const uint32_t frequency);
void PWM1_Set_Duty(uint8_t duty_ratio);
void PWM1_Start();
void PWM1_Stop();


#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

