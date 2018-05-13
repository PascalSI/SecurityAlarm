///******************************************************************************/
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

#include "pwm.h"
#include "keypad.h"

#include "1wire-config.h"
#include "1wire.h"
#include "pc2keyboard.h"

//#define useInverseInputSensors
#define usePWM
#define useDebugRS232
#define useVoltageDetector
//#define useIBUTTON
#define useKeyboard
//#define usePC2Keyboard

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
#define LED_SYS1_ON 1U
#define LED_SYS1_OFF !LED_SYS1_ON
#define LED_SYS1_TRIS TRISCbits.TRISC2

#define LED_DOOR LATPORTC.bits.RC5
#define LED_DOOR_ON 0U
#define LED_DOOR_OFF !LED_DOOR_ON
#define LED_DOOR_TRIS TRISCbits.TRISC5
#define LEDS_FLUSH_PORT LATPORTC_FLUSH

// BUZZER
#define BUZZER LATPORTC.bits.RC4
#define BUZZER_ON 1U
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
//ROW4
#define KEYPAD_ROW4 LATPORTC.bits.RC7
#define KEYPAD_ROW4_ON 0U
#define KEYPAD_ROW4_OFF !KEYPAD_ROW4_ON
#define KEYPAD_ROW4_TRIS TRISCbits.TRISC7
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
#define DOOR_BUTTON_WRITE LATPORTC.bits.RC6
#define DOOR_BUTTON_ON 0
#define DOOR_BUTTON_OFF !DOOR_BUTTON_ON
#define DOOR_BUTTON_TRIS TRISCbits.TRISC6
#define DOOR_BUTTON_FLUSH_PORT LATPORTC_FLUSH

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

//------------

bit ToneNext = false;
bit TonePaused = false;
bit ToneEnabled = false;
bit TonePlayOnce = false;

//SOS melody
const uint8_t Melody00[] = {
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
const uint8_t Melody01[] = {
    45, 15
};
//Melody seconds Bit
const uint8_t Melody02[] = {
    5, 125
};
//Melody ALARM OFF
const uint8_t Melody03[] = {
    1, 150,
    150, 50,
    5, 20,
    5, 5
};

//Melody ALARM ON
const uint8_t Melody04[] = {
    50, 50,
    5, 50,
    5, 50,
    5, 50,
    5, 1
};

//Melody ERROR
const uint8_t Melody05[] = {
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



const uint8_t passwordCode[] = {
    20, 20, 20
};

#ifdef useKeyboard
#define maxPinCodes 2
const uint8_t pinCodes[maxPinCodes][4] = {
    {'1','2','3','4'},   
    {'4','3','2','1'}
};

uint8_t passwordCodeCurrnetPointer = 0;
uint8_t pinCodeCurrnetPointer = 0;
uint8_t pinCodeCurrent[4];
#endif

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

bit DOOR_Last_State;
bit DOOR_BUTTON_Last_State;
bit LOCK1_Last_State;
bit LOCK2_Last_State;

bit DOOR_State;
bit DOOR_BUTTON_State;
bit LOCK1_State;
bit LOCK2_State;


#ifdef useDebugRS232
bit LOCK1_State_Displayed;
bit LOCK2_State_Displayed;
bit DOOR_State_Displayed;
#endif

int16_t DOOR_LastDebounceTime;
int16_t DOOR_BUTTON_LastDebounceTime;
int16_t LOCK1_LastDebounceTime;
int16_t LOCK2_LastDebounceTime;

int16_t DOOR_BUTTON_LastStateTime;
int16_t DOOR_BUTTON_Duration; //0.1s = 1 
Keypad_State_e DOOR_BUTTON_FinalState = KEYPAD_UP;

const uint8_t debounceDelay = 200U;

int16_t delaySysLEDblink = 0;
int16_t delayone_pwm = 0;
int16_t buzzer_delay = 0;
uint16_t buzzer_duration = 0;
int16_t buzzer_pause_delay = 0;
int16_t buzzer_pause_duration = 0;

//PWM
const uint32_t PWM_frequency = 2000u;
uint8_t PWM_duty_cycle = 50;

#define pwm_FlashMode_middle 100
#define pwm_FlashMode_long  255
#define pwm_FlashMode_short 5


uint8_t delayone_pwm2_max = pwm_FlashMode_long;
uint16_t delayone_pwm2 = 0;
#define pwm_duty_step 5
#define pwm_max_duty_value 95           //MIN brigtness 100-off, 0-on
#define pwm_min_duty_value 0            //MAX brigtness
#define pwm_min_duty_value_lowPower 75  //MAX brigtness in lowpower mode
uint8_t pwm_direction = pwm_duty_step;

//LED blink
#define sysLEDblinkDelayOn 10 //0.1 sec
#define sysLEDblinkDelayOff 1000-sysLEDblinkDelayOn //1 sec
#define sysLEDblinkDelayOff_lowPower (1000*5)-sysLEDblinkDelayOn //5 sec

//Security_State

typedef enum _Security_State_e {
    DELAY_PREPARE_DOOR = 0,
    ARMED,
    DISARMED,
//    UNLOCKED,
//    WAIT_OPENING_ALL_LOCKS,
    DOOR_OPENED_DELAY,
    ALARM,
    ALARM_TIMEOUT,
    ALARM_UNLOCKING_TIMEOUT
} Security_State_e;

typedef enum _lock_Security_State_e {
    LOCKED,
    UNLOCKED,
    WAIT_OPENING_ALL_LOCKS,
} Lock_Security_State_e;


#define Time_for_Unlocking 1l*60l*1000L
int32_t delay_for_Unlocking;

#define Time_for_CloseDoor 45L*1000L
int32_t delay_for_CloseDoor;

#define Time_for_OpenDoor 1L*45L*1000L
int32_t delay_for_OpenDoor;

#define Time_for_Alarm 5L*60L*1000L
int32_t delay_for_Alarm;

#define Time_for_DISARMED 2L*60L*1000L
#define Time_for_DISARMED_LONG 30L*60L*1000L
int32_t delay_for_DISARMED;
bit DISARMED_LONG=false;

Security_State_e Security_State = ARMED;
Lock_Security_State_e Lock_Security_State = UNLOCKED;

bit AuthPasswordOK;
#define Time_for_Auth 5L*1000L //5 sec
int16_t delay_for_Auth;

#ifdef useIBUTTON

//

//eeprom unsigned char EiButton_PASSWORDS=2;

#define MAX_iButton_PASSWORDS 30

typedef struct EEiButtonSN_ {
    unsigned char iButtonSN[8];
} EEiButtonSN;

//eeprom EEiButtonSN EiButtons[MAX_iButton_PASSWORDS] = {
////     Precomiled to EPPROM iButton Passwords    
////    {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x88},
////    {0x03, 0xC4, 0x05, 0x06, 0x07, 0x08, 0x00, 0x99} 
//#include "ibutton_passwords.inc"
//};

typedef struct EEiButtons_ {
    unsigned char total_stored;
    EEiButtonSN EiButtons[MAX_iButton_PASSWORDS];
    //    = {
    //        //     Precomiled to EPPROM iButton Passwords    
    //        //    {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00, 0x88},
    //        //    {0x03, 0xC4, 0x05, 0x06, 0x07, 0x08, 0x00, 0x99} 
    //#include "ibutton_passwords.inc"
    //    }
} EEiButtons;

#define EiButton_included_count_paswords 3
eeprom EEiButtons mEEiButtons = {
    EiButton_included_count_paswords,
    {
#include "ibutton_passwords.inc"        
    }
};
#define EiButton_MASTER_KEY_ID 1

//eeprom unsigned char EiButton_PASSWORDS = sizeof (EiButtons) / sizeof (EEiButtonSN);

unsigned char serial_number[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

bit needAddSerialNumberToEPPROM;
bit needClearSerialNumberToEPPROM;
bit isMasterIButton;
//int32_t delay_for_iButton=0;
//uint8_t try_for_iButton=0;
//#define  MAX_TRY_FOR_iButton 3u


#endif




//------------



void Timer0_Init(void);
int16_t millis(void);
int32_t millis_32(void);


void InitApp(void); /* I/O and Peripheral Initialization */

//Melody 
void buzzer_process(void);

void Melody_Play();
void Melody_Stop();
void Melody_Select(MelodyCode_e id);
void Melody_Processes(void);


void scan_sensors(void);
Keypad_State_e getDoorButtonState();

//PWM
void pwm_flash_mode(uint8_t m);
void pwm_process(void);

void sysLED_process(void);

//ibutton 
#ifdef useIBUTTON



unsigned char Detect_Slave_Device(void);
unsigned char CheckSerialNumberinEPPROM(unsigned char *sn);
unsigned char AddSerialNumberToEPPROM(unsigned char *sn);
void ClearSerialNumbersOfEPPROM(void);
#endif



//UART
char UART_Init(const long int baudrate);
void UART_Write(char data);
void UART_Write_Next(char data);
void UART_Write2byte(int16_t data);
char UART_TX_Empty();
void UART_Write_Text(const char *text);
char UART_Data_Ready();
char UART_Read();
void UART_Read_Text(char *Output, uint8_t length);

void VoltageDetector_Start(void);

#ifdef useKeyboard
uint8_t checkPinCode(uint8_t key);
#endif



#endif