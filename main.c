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
#include <stdbool.h>       /* For true/false definition */

#include "interrupts.h"
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include "pwm.h"
#include "keypad.h"

#include "1wire-config.h"
#include "1wire.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

#define usePWM
#define useDebugRS232
//#define useIBUTTON
//#define useKeyboard

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

static bit ToneNext = false;
static bit TonePaused = false;
static bit ToneEnabled = false;
static bit TonePlayOnce = false;
//SOS melody
const uint8_t static Melody00[] = {
    30, 15,
    90, 30,
    30, 15,
    90, 30,
    30, 15,
    90, 30,
    30, 200,
    15, 7,
    15, 7,
    15, 30,
    30, 7,
    30, 7,
    30, 30,
    15, 7,
    15, 7,
    15, 200
};
//Melody Siren
const uint8_t static Melody01[] = {
    45, 15
};
//Melody seconds Bit
const uint8_t static Melody02[] = {
    5, 250
};
//Melody ALARM OFF
const uint8_t static Melody03[] = {
    150, 50,
    5, 20,
    5, 5
};

//Melody ALARM ON
const uint8_t static Melody04[] = {
    50, 50,
    5, 50,
    5, 50,
    5, 50,
    5, 1
};

//Melody ERROR
const uint8_t static Melody05[] = {
    1, 1,
    1, 1,
    1, 1,
    1, 1,
    1, 1
};

typedef enum _MelodyCod_e {
    SOS_e = 0,
    SIREN_e,
    SECONDS_e,
    ALARMOFF_e,
    ALARMON_e,
    ERROR_e
} MelodyCode_e;

const uint8_t* currentMelody;
uint8_t currentMelodyLength = sizeof (Melody00);
uint8_t currentMelodyPointer = 0;
MelodyCode_e currentMelodyID = 0;


const unsigned char key1[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char serial_number[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t static passwordCode[] = {
    20, 20, 20
};

uint8_t passwordCodeCurrnetPointer = 0;

typedef enum _Sensors_e {
    LOCK1_e = 0,
    LOCK2_e,
    DOOR_BUTTON_e,
    DOOR_e
} Sensors_e;

typedef struct _Sensor_s {
    //  bit Last_State;             
    //  bit State;              
    uint16_t LastDebounceTime;
} Sensor_s;

//Sensor_s Sensors[4];

static bit DOOR_Last_State;
static bit DOOR_BUTTON_Last_State;
static bit LOCK1_Last_State;
static bit LOCK2_Last_State;

static bit DOOR_State;
static bit DOOR_BUTTON_State;
static bit LOCK1_State;
static bit LOCK2_State;

int16_t DOOR_LastDebounceTime;
int16_t DOOR_BUTTON_LastDebounceTime;
int16_t LOCK1_LastDebounceTime;
int16_t LOCK2_LastDebounceTime;

int16_t DOOR_BUTTON_LastStateTime;
int16_t DOOR_BUTTON_Duration; //0.1s = 1 
Keypad_State_e DOOR_BUTTON_FinalState = KEYPAD_UP;

const uint8_t debounceDelay = 100;

int16_t delaySysLEDblink = 0;
int16_t delayone_pwm = 0;
int16_t buzzer_delay = 0;
uint16_t buzzer_duration = 0;
int16_t buzzer_pause_delay = 0;
uint16_t buzzer_pause_duration = 0;

void buzzer_process(void) {
    //BUZZER PLAY
    if (BUZZER == BUZZER_ON) {
        if ((millis() - buzzer_delay) >= buzzer_duration) {
            //timeout for on state
            BUZZER = BUZZER_OFF;
            BUZZER_FLUSH_PORT;
            buzzer_duration = 0;
            ToneNext = true;
        }
    } else {
        if (buzzer_duration) {
            buzzer_delay = millis();
            BUZZER = BUZZER_ON;
            BUZZER_FLUSH_PORT;
        }
    }
}

void PlayMelody() {
    ToneNext = true;
    ToneEnabled = true;
}

void StopMelody() {
    ToneEnabled = false;
}

void SelectMelody(MelodyCode_e id) {
    if (id <= ERROR_e) {
        StopMelody();
        currentMelodyPointer = 0;
        currentMelodyID = id;
        switch (currentMelodyID) {
            case SOS_e:
                currentMelody = Melody00;
                currentMelodyLength = sizeof (Melody00);
                break;
            case SIREN_e:
                currentMelody = Melody01;
                currentMelodyLength = sizeof (Melody01);
                break;
            case SECONDS_e:
                currentMelody = Melody02;
                currentMelodyLength = sizeof (Melody02);
                break;
            case ALARMOFF_e:
                currentMelody = Melody03;
                currentMelodyLength = sizeof (Melody03);
                TonePlayOnce = true;
                break;
            case ALARMON_e:
                currentMelody = Melody04;
                currentMelodyLength = sizeof (Melody04);
                TonePlayOnce = true;
                break;
            case ERROR_e:
                currentMelody = Melody05;
                currentMelodyLength = sizeof (Melody05);
                TonePlayOnce = true;
                break;
        }

        PlayMelody();
    }
}

void melody_processes(void) {
    if (!TonePaused && buzzer_pause_duration > 0) {
        TonePaused = true;
        buzzer_pause_delay = millis();
    }
    if (buzzer_pause_duration > 0 && ((millis() - buzzer_pause_delay) >= buzzer_pause_duration)) {
        buzzer_pause_duration = 0;
        TonePaused = false;
        ToneNext = true;
    }

    if (ToneNext) {
        ToneNext = false;
        if (ToneEnabled) {
            uint16_t duration = currentMelody[currentMelodyPointer]*10u;
            if (currentMelodyPointer % 2) {
                buzzer_pause_duration = duration;
            } else {
                buzzer_duration = duration;
            }
            currentMelodyPointer++;
            if (currentMelodyPointer >= currentMelodyLength) {
                currentMelodyPointer = 0;
                if (TonePlayOnce) {
                    ToneEnabled = false;
                    TonePlayOnce = false;
                }
            }
        }
    }
}

void scan_sensors(void) {

    //------
    if (LOCK1 != LOCK1_Last_State) {
        LOCK1_LastDebounceTime = millis();
    }

    if ((millis() - LOCK1_LastDebounceTime) > debounceDelay) {
        if (LOCK1 != LOCK1_State) {
            LOCK1_State = LOCK1;
        }
    }

    //------
    if (LOCK2 != LOCK2_Last_State) {
        LOCK2_LastDebounceTime = millis();
    }

    if ((millis() - LOCK2_LastDebounceTime) > debounceDelay) {
        if (LOCK2 != LOCK2_State) {
            LOCK2_State = LOCK2;
        }
    }


    //------
#ifndef useIBUTTON  
    if (DOOR_BUTTON != DOOR_BUTTON_Last_State) {
        DOOR_BUTTON_LastDebounceTime = millis();
    }

    if ((millis() - DOOR_BUTTON_LastDebounceTime) > debounceDelay) {
        if (DOOR_BUTTON != DOOR_BUTTON_State) {
            DOOR_BUTTON_State = DOOR_BUTTON;
            if (DOOR_BUTTON_State == DOOR_BUTTON_ON) {
                DOOR_BUTTON_FinalState = KEYPAD_PRESSED;
            } else {
                DOOR_BUTTON_FinalState = KEYPAD_RELEASED;
            }
            DOOR_BUTTON_Duration = (millis() - DOOR_BUTTON_LastStateTime) / 100;
            DOOR_BUTTON_LastStateTime = millis();
        }
    }

#endif    

    //------
    if (DOOR != DOOR_Last_State) {
        DOOR_LastDebounceTime = millis();
    }

    if ((millis() - DOOR_LastDebounceTime) > debounceDelay) {
        if (DOOR != DOOR_State) {
            DOOR_State = DOOR;
        }
    }

    LOCK1_Last_State = LOCK1;
    LOCK2_Last_State = LOCK2;
    DOOR_BUTTON_Last_State = DOOR_BUTTON;
    DOOR_Last_State = DOOR;
}

Keypad_State_e getDoorButtonState() {
    Keypad_State_e tmp = KEYPAD_UP;
    if (DOOR_BUTTON_FinalState != KEYPAD_UP) {
        tmp = DOOR_BUTTON_FinalState;
        DOOR_BUTTON_FinalState = KEYPAD_UP;
    }
    return tmp;
}


//PWM
const uint32_t PWM_frequency = 2000u;
uint8_t PWM_duty_cycle = 50;

#define pwm_FlashMode_middle 100
#define pwm_FlashMode_long  255
#define pwm_FlashMode_short 5


uint8_t delayone_pwm2_max = pwm_FlashMode_long;
uint8_t delayone_pwm2 = 0;
#define pwm_duty_step 5
#define pwm_max_duty_value 95
#define pwm_min_duty_value 0
int8_t pwm_direction = pwm_duty_step;

void pwm_flash_mode(uint8_t m) {
    delayone_pwm2_max = m;
}

void pwm_process(void) {
#ifdef usePWM
    if (millis() - delayone_pwm > 10) {
        delayone_pwm = millis();
        if (PWM_duty_cycle >= pwm_max_duty_value) {
            pwm_direction = -pwm_duty_step;
        }
        if (PWM_duty_cycle <= pwm_min_duty_value) {
            pwm_direction = pwm_duty_step;
        }
        if (PWM_duty_cycle == pwm_max_duty_value) {
            delayone_pwm2++;
        }

        if (delayone_pwm2 > 0) {
            if (delayone_pwm2 >= delayone_pwm2_max) {
                delayone_pwm2 = 0;
                PWM_duty_cycle = PWM_duty_cycle + pwm_direction;
            }
        } else {
            PWM_duty_cycle = PWM_duty_cycle + pwm_direction;
            PWM1_Stop();
            PWM1_Set_Duty(PWM_duty_cycle);
            PWM1_Start();
        }
    }
#endif
}

#define sysLEDblinkDelayOff 990 //1 sec
#define sysLEDblinkDelayOn 10 //0.1 sec

void sysLED_process(void) {
    if (millis() - delaySysLEDblink >= ((LED_SYS1) ? sysLEDblinkDelayOn : sysLEDblinkDelayOff)) {
        delaySysLEDblink = millis();
        LED_SYS1 = ~LED_SYS1;
        LEDS_FLUSH_PORT;
    }
}

//----------------------------------------------------------------------------//
// Function:        unsigned char Detect_Slave_Device(void)
// PreCondition:    None
// Input:           None
// Output:          1 - Not Present   0 - Present
// Overview:        To check the presence of slave device.
//----------------------------------------------------------------------------//

unsigned char Detect_Slave_Device(void) {
    if (!OW_reset_pulse()) {
        return OW_HIGH;
    } else {
        return OW_LOW;
    }

}

typedef enum _Security_State_e {
    DELAY_PREPARE_DOOR = 0,
    ARMED,
    DISARMED,
    UNLOCKED,
    WAIT_OPENING_ALL_LOCKS,
    DOOR_OPENED_DELAY,
    ALARM,
    ALARM_TIMEOUT,
    ALARM_UNLOCKING_TIMEOUT
} Security_State_e;


#define Time_for_Unlocking 1u*60u*1000u
int32_t delay_for_Unlocking;

#define Time_for_CloseDoor 45u*1000u
int32_t delay_for_CloseDoor;

#define Time_for_OpenDoor 1u*60u*1000u
int32_t delay_for_OpenDoor;

#define Time_for_Alarm 5u*60u*1000u
int32_t delay_for_Alarm;

#define Time_for_DISARMED 2u*60u*1000u
int32_t delay_for_DISARMED;

Security_State_e Security_State = ARMED;

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();


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
        melody_processes();
        pwm_process();
        scan_sensors();

        switch (getDoorButtonState()) {
            case KEYPAD_RELEASED:
                //passwordCode[passwordCodeCurrnetPointer]
                if (Security_State == DOOR_OPENED_DELAY) {
                    StopMelody();
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
                        SelectMelody(SECONDS_e);
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
                    SelectMelody(ALARMOFF_e);
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
                            SelectMelody(ERROR_e);
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
                SelectMelody(SECONDS_e);
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


        if ((Security_State == DISARMED) && ((millis_32() - delay_for_DISARMED) >= Time_for_DISARMED)) {
            Security_State = ARMED;
            pwm_flash_mode(pwm_FlashMode_long);
            SelectMelody(ALARMON_e);
#ifdef useDebugRS232            
            UART_Write_Text("Time_for_DISARMED. ARMED\n");
#endif   
        }

        if ((Security_State == DELAY_PREPARE_DOOR) && ((millis_32() - delay_for_CloseDoor) >= Time_for_CloseDoor)) {
            Security_State = ARMED;
#ifdef useDebugRS232            
            UART_Write_Text("Time_for_CloseDoor. ARMED\n");
#endif                
            pwm_flash_mode(pwm_FlashMode_long);
            SelectMelody(ALARMON_e);
        }

        if ((Security_State == WAIT_OPENING_ALL_LOCKS) && ((millis_32() - delay_for_Unlocking) >= Time_for_Unlocking)) {
            Security_State = ALARM_UNLOCKING_TIMEOUT;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            SelectMelody(SIREN_e);
#ifdef useDebugRS232            
            UART_Write_Text("WAIT_OPENING_ALL_LOCKS. ALARM_UNLOCKING_TIMEOUT\n");
#endif   
        }

        if ((Security_State == DOOR_OPENED_DELAY) && ((millis_32() - delay_for_OpenDoor) >= Time_for_OpenDoor)) {
            Security_State = ALARM;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            SelectMelody(SOS_e);
#ifdef useDebugRS232            
            UART_Write_Text("DOOR_OPENED_DELAY. ALARM SOS\n");
#endif   
        }

        if ((Security_State == ALARM || Security_State == ALARM_UNLOCKING_TIMEOUT) && ((millis_32() - delay_for_Alarm) >= Time_for_Alarm)) {
            Security_State = ARMED;
            pwm_flash_mode(pwm_FlashMode_long);
            StopMelody();
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
        
      
        if (Detect_Slave_Device()!=OW_HIGH) {
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



        /* TODO <INSERT USER APPLICATION CODE HERE> */
    }

}

