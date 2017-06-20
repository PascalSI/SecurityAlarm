/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "interrupts.h"
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include "pwm.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

#define usePWR

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/
void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    //SerialWrite();

    ei();


    int16_t delayone = millis();
    int16_t delayone_pwm = millis();
    int16_t delay_buzzer;
    uint8_t buzzer_duration = 0;
#define delayone_pwm2_max 100
    int delayone_pwm2 = 0;


    UART_Init(1200);

    const uint32_t frequency = 2000u;
    uint8_t duty_cycle = 50;
    //Initialize_IO ();
#ifdef usePWR
    PWM1_Init(frequency);
    PWM1_Set_Duty(duty_cycle);
    PWM1_Start();
#endif    

    UART_Write_Text("System initialized\n");

#define pwm_duty_step 5
#define pwm_max_duty_value 95
#define pwm_min_duty_value 0

    int8_t pwm_direction = pwm_duty_step;

    while (1) {


        if (millis() - delayone > 1000) {
            delayone = millis();
            LED_SYS1 = ~LED_SYS1;
            LEDS_FLUSH_PORT;
            if (buzzer_duration) {
                BUZZER = BUZZER_ON;
                BUZZER_FLUSH_PORT;
                delay_buzzer = millis();
            }

        }
#ifdef usePWR
        if (millis() - delayone_pwm > 10) {
            delayone_pwm = millis();
            if (duty_cycle >= pwm_max_duty_value) {
                pwm_direction = -pwm_duty_step;
            }
            if (duty_cycle <= pwm_min_duty_value) {
                pwm_direction = pwm_duty_step;
            }
            if (duty_cycle == pwm_max_duty_value) {
                delayone_pwm2++;
            }

            if (delayone_pwm2 > 0) {
                if (delayone_pwm2 >= delayone_pwm2_max) {
                    delayone_pwm2 = 0;
                    duty_cycle = duty_cycle + pwm_direction;
                }
            } else {
                duty_cycle = duty_cycle + pwm_direction;
                PWM1_Stop();
                PWM1_Set_Duty(duty_cycle);
                PWM1_Start();
            }
        }
#endif
        
        if (BUZZER == BUZZER_ON) {
            if (millis() - delay_buzzer >= buzzer_duration) {
                BUZZER = BUZZER_OFF;
                BUZZER_FLUSH_PORT;
                buzzer_duration=0;
            }
        }





        //        LED_SYS1 = !LED_SYS1;
        //        LEDS_FLUSH_PORT;
        //
        //        __delay_ms(10000);

        //        LED_SYS1 = !LED_SYS1;
        //LED_DOOR = (DOOR_BUTTON == DOOR_BUTTON_ON) ? (LED_DOOR_ON) : (LED_DOOR_OFF);

        //        if (LED_DOOR == LED_DOOR_ON) {
        //            UART_Write2byte(millis());
        //            LEDS_FLUSH_PORT;
        //        }
        if (DOOR_BUTTON == DOOR_BUTTON_ON) {
            buzzer_duration = 5;
            UART_Write_Text("DOOR_BUTTON\n");
        } 

        if (LOCK1 == LOCK1_ON) {
            buzzer_duration = 10;
            UART_Write_Text("LOCK1\n");
        } 
        if (LOCK2 == LOCK2_ON) {
            buzzer_duration = 10;
            UART_Write_Text("LOCK2\n");
        } 
        if (DOOR == DOOR_ON) {
            buzzer_duration = 10;
            UART_Write_Text("DOOR\n");
        } 


        if (!UART_TX_Empty()){
                __delay_ms(200);
        }



        /* TODO <INSERT USER APPLICATION CODE HERE> */
    }

}

