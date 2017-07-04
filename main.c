/******************************************************************************/
/* Files to Include                                                           */
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


#include "interrupts.h"
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */



/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/



void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

#ifdef usePC2Keyboard
    PC2Keyboard_Init();
#endif     

    //Initialize_Keypad();

    //SerialWrite();

    DOOR_Last_State = DOOR_OFF;
    DOOR_BUTTON_Last_State = DOOR_BUTTON_OFF;
    LOCK1_Last_State = LOCK1_OFF;
    LOCK2_Last_State = LOCK2_OFF;

    DOOR_State = DOOR_OFF;
    DOOR_BUTTON_State = DOOR_BUTTON_OFF;
    LOCK1_State = LOCK1_OFF;
    LOCK2_State = LOCK2_OFF;



    delaySysLEDblink = millis();
    delayone_pwm = millis();
    buzzer_delay = 0;
    buzzer_duration = 0;
    buzzer_pause_delay = 0;
    buzzer_pause_duration = 0;



#ifdef useDebugRS232
    UART_Init(1200);
#endif    


    //Initialize_IO ();
#ifdef usePWM
    PWM1_Init(PWM_frequency);
    PWM1_Set_Duty(PWM_duty_cycle);
    PWM1_Start();
#endif    

#ifdef useDebugRS232    
    UART_Write_Text("Init. ARMED\n");
#endif    



    BUZZER = BUZZER_OFF;
    BUZZER_FLUSH_PORT;


    ei();

    while (1) {

        CLRWDT();

        buzzer_process();

        sysLED_process();
        Melody_Processes();
        pwm_process();
        scan_sensors();

        switch (getDoorButtonState()) {
            case KEYPAD_RELEASED:
                //passwordCode[passwordCodeCurrnetPointer]
                if (Security_State == DOOR_OPENED_DELAY) {
                    Melody_Stop();
                }

                if (Security_State == DOOR_OPENED_DELAY \
                        || Security_State == ALARM \
                        || Security_State == ALARM_UNLOCKING_TIMEOUT) {
                    if ((DOOR_BUTTON_Duration >= passwordCode[passwordCodeCurrnetPointer]) \
                            && (DOOR_BUTTON_Duration <= (passwordCode[passwordCodeCurrnetPointer] + 10))) {
                        //step of password  OK
                        passwordCodeCurrnetPointer++;
                        buzzer_duration = 20;
                    } else {
                        //wrong step of password
                        passwordCodeCurrnetPointer = 0;
                        //SelectMelody(ERROR_e);
                        buzzer_duration = 500;
                    }
                }

                if (Security_State == ARMED || Security_State == DISARMED) {
                    if (DOOR_BUTTON_Duration >= 5 && DOOR_BUTTON_Duration < 20) {
                        //delayone_pwm2_max = delayone_pwm2_short;
                        Melody_Select(SECONDS_e);
                        delay_for_CloseDoor = millis_32();
                        Security_State = DELAY_PREPARE_DOOR;
#ifdef useDebugRS232            
                        UART_Write_Text("DELAY_PREPARE_DOOR\n");
#endif 
                    } else {
                        //StopMelody();
                        //delayone_pwm2_max = delayone_pwm2_middle;
                    }
                }

                //Check password OK is ?
                if (passwordCodeCurrnetPointer >= sizeof (passwordCode)) {
                    //password ok
#ifdef useDebugRS232            
                    UART_Write_Text("DISARMED\n");
#endif 
                    Security_State = DISARMED;
                    delay_for_DISARMED = millis_32();
                    Melody_Select(ALARMOFF_e);
                    pwm_flash_mode(pwm_FlashMode_short);
                    passwordCodeCurrnetPointer = 0;
                }
                break;
            case KEYPAD_PRESSED:
                if (Security_State == ARMED) {
                    if ((passwordCodeCurrnetPointer > 0)) {
                        if ((DOOR_BUTTON_Duration >= passwordCode[passwordCodeCurrnetPointer]) && \
                        (DOOR_BUTTON_Duration <= passwordCode[passwordCodeCurrnetPointer] + 10)) {
                            passwordCodeCurrnetPointer++;
                            if (passwordCodeCurrnetPointer >= sizeof (passwordCode)) {
                                passwordCodeCurrnetPointer = 0;
                            }
                            buzzer_duration = 5;
                        } else {
                            passwordCodeCurrnetPointer = 0;
                            Melody_Select(ERROR_e);
                        }
                    } else {
                        //default beep
                        buzzer_duration = 5;
                    }
                } else {
                    buzzer_duration = 5;
                }
                //SelectMelody(1);
                break;
        }


        if (DOOR_State == DOOR_OFF && Security_State != DOOR_OPENED_DELAY) {
            if (LOCK1_State == LOCK1_ON) {
                //buzzer_duration = 10;

#ifdef useDebugRS232            
                UART_Write_Text("LOCK1\n");
#endif            
            }
            if (LOCK2_State == LOCK2_ON) {
                //buzzer_duration = 10;
#ifdef useDebugRS232            
                UART_Write_Text("LOCK2\n");
#endif            
            }

            if (LOCK1_State == LOCK1_ON && LOCK2_State == LOCK2_ON) {
                if (Security_State != UNLOCKED) {
                    Security_State = UNLOCKED;
#ifdef useDebugRS232                   
                    UART_Write_Text("UNLOCKED\n");
#endif                 
                }
            } else if (LOCK1_State == LOCK1_ON || LOCK2_State == LOCK2_ON) {
                if (Security_State != WAIT_OPENING_ALL_LOCKS) {
                    Security_State = WAIT_OPENING_ALL_LOCKS;
                    delay_for_Unlocking = millis_32();
#ifdef useDebugRS232                   
                    UART_Write_Text("WAIT_OPENING_ALL_LOCKS\n");
#endif                 
                }
            }
        }


        if (DOOR_State == DOOR_ON) {
            //buzzer_duration = 10;
#ifdef useDebugRS232            
            UART_Write_Text("DOOR\n");
#endif       

            if (Security_State != DOOR_OPENED_DELAY) {
                Security_State = DOOR_OPENED_DELAY;
                delay_for_OpenDoor = millis_32();
                Melody_Select(SECONDS_e);
#ifdef useDebugRS232                   
                UART_Write_Text("DOOR_OPENED_DELAY\n");
#endif                 
            }

        }

#ifdef useDebugRS232
        //        if (!UART_TX_Empty()){
        //                __delay_ms(200);
        //        }
#endif        

#ifdef useKeyboard        
        uint8_t key; //= getKey();
        //    ROW_1_PIN = KEYPAD_ROW1_ON;
        //    ROW_2_PIN = KEYPAD_ROW2_ON;
        //    ROW_3_PIN = KEYPAD_ROW3_ON;
        //    KEYPAD_ROWS_FLUSH_PORT;
        //    NOP();
        //uint8_t key=PORTA;

        if (key > 0 && key != '1') {
            buzzer_duration = 10;
#ifdef useDebugRS232                        
            UART_Write(key);
#endif                       
        }
#endif
        //check timers


        if ((Security_State == DISARMED)) {
            if ((millis_32() - delay_for_DISARMED) >= Time_for_DISARMED) {
                Security_State = ARMED;
                pwm_flash_mode(pwm_FlashMode_long);
                Melody_Select(ALARMON_e);
#ifdef useDebugRS232            
                UART_Write_Text("Time_for_DISARMED. ARMED\n");
            }
#endif   
        }

        if ((Security_State == DELAY_PREPARE_DOOR) && ((millis_32() - delay_for_CloseDoor) >= Time_for_CloseDoor)) {
            Security_State = ARMED;
#ifdef useDebugRS232            
            UART_Write_Text("Time_for_CloseDoor. ARMED\n");
#endif                
            pwm_flash_mode(pwm_FlashMode_long);
            Melody_Select(ALARMON_e);
        }

        if ((Security_State == WAIT_OPENING_ALL_LOCKS) && ((millis_32() - delay_for_Unlocking) >= Time_for_Unlocking)) {
            Security_State = ALARM_UNLOCKING_TIMEOUT;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            Melody_Select(SIREN_e);
#ifdef useDebugRS232            
            UART_Write_Text("WAIT_OPENING_ALL_LOCKS. ALARM_UNLOCKING_TIMEOUT\n");
#endif   
        }

        if ((Security_State == DOOR_OPENED_DELAY) && ((millis_32() - delay_for_OpenDoor) >= Time_for_OpenDoor)) {
            Security_State = ALARM;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            Melody_Select(SOS_e);
#ifdef useDebugRS232            
            UART_Write_Text("DOOR_OPENED_DELAY. ALARM SOS\n");
#endif   
        }

        if ((Security_State == ALARM || Security_State == ALARM_UNLOCKING_TIMEOUT) && ((millis_32() - delay_for_Alarm) >= Time_for_Alarm)) {
            Security_State = ARMED;
            pwm_flash_mode(pwm_FlashMode_long);
            Melody_Stop();
#ifdef useDebugRS232            
            UART_Write_Text("ALARM_TIMEOUT. ARMED\n");
#endif   
        }

#ifdef useIBUTTON        
        // Is slave present???
        //            OW_WRITE_PIN=1;
        //            OW_FLUSH_PIN;
        //            OW_PIN_DIRECTION = OW_OUTPUT;            
        //            __delay_us(100);


        if (Detect_Slave_Device() != OW_HIGH) {
            //DOOR_BUTTON = 0;
            //drive_OW_low();
            //OW_WRITE_PIN=0;
            //OW_FLUSH_PIN;
        } else {
#ifdef useDebugRS232            
            UART_Write_Text("Detect_Slave\n");
#endif 
            //DOOR_BUTTON = 1;
            //drive_OW_high();

            OW_write_byte(0x33); // Send a command to read a serial number

            for (uint8_t temp = 0; temp < 8; temp++) {
                serial_number[temp] = OW_read_byte(); // Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device
            }
#ifdef useDebugRS232    
            for (uint8_t temp = 0; temp < 8; temp++) {
                UART_Write(serial_number[temp]);
            }
#endif
        }
#endif

#ifdef usePC2Keyboard
        PC2Keboard_Process();
#endif  


        /* TODO <INSERT USER APPLICATION CODE HERE> */
    }

}

