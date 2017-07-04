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

void Melody_Play() {
    ToneNext = true;
    ToneEnabled = true;
}

void Melody_Stop() {
    ToneEnabled = false;
}

void Melody_Select(MelodyCode_e id) {
    if (id <= ERROR_e) {
        Melody_Stop();
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

        Melody_Play();
    }
}

void Melody_Processes(void) {
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

void sysLED_process(void) {
    if (millis() - delaySysLEDblink >= ((LED_SYS1) ? sysLEDblinkDelayOn : sysLEDblinkDelayOff)) {
        delaySysLEDblink = millis();
        LED_SYS1 = ~LED_SYS1;
        LEDS_FLUSH_PORT;

#ifdef useVoltageDetector
        if (LED_SYS1) VoltageDetector_Start();
#endif
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

void InitApp(void) {
    /* TODO Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Enable interrupts */

    //ANALOG PORT SET TO DIGITAL
    ANSEL = 0b00000000;
    ANSELH = 0b00000000;



    LED_SYS1_TRIS = TRIS_OUTPUT;
    LED_DOOR_TRIS = TRIS_OUTPUT;
    LED_SYS1 = LED_SYS1_OFF;
    LED_DOOR = LED_DOOR_OFF;
    LEDS_FLUSH_PORT;

    BUZZER_TRIS = TRIS_OUTPUT; //TRIS_INPUT;//
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
    OPTION_REGbits.nRABPU = 0; //0 = PORTA/PORTB pull-ups are enabled by individual port latch values
    WPUA = 0b11111111;


    //SENSORS OF DOOR
    DOOR_BUTTON_TRIS = TRIS_INPUT;
    LOCK1_TRIS = TRIS_INPUT;
    LOCK2_TRIS = TRIS_INPUT;
    DOOR_TRIS = TRIS_INPUT;


    //WDT INIT
    //31kHz / 512 = 16 ms
    //31kHz / 65536 = 2.11 s
    WDTCONbits.SWDTEN = 1;
    WDTCONbits.WDTPS = 0b1011;
    //0000 = 1:32
    //0001 = 1:64
    //0010 = 1:128
    //0011 = 1:256
    //0100 = 1:512 (Reset value)
    //0101 = 1:1024
    //0110 = 1:2048
    //0111 = 1:4096
    //1000 = 1:8192
    //1001 = 1:16384
    //1010 = 1:32768
    //1011 = 1:65536
    OPTION_REGbits.PSA = 0; //Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0



    Timer0_Init();

    // Interrupt Registers
    INTCON = 0; // clear the interrpt control register
    INTCONbits.T0IE = 1; // bit5 TMR0 Overflow Interrupt Enable bit...1 = Enables the TMR0 interrupt
    INTCONbits.T0IF = 0; // bit2 clear timer 0 interrupt flag
    INTCONbits.GIE = 1; // bit7 global interrupt enable


#ifdef useVoltageDetector
    VRCONbits.VP6EN = 1; // ENABLE VP 0.6 Voltage reference
    ADCON0bits.ADFM = 1; // Right justify result
    ADCON0bits.CHS = 0b1101; // VP6 is ADC input
    ADCON0bits.VCFG = 0; // VCFG: Voltage Reference bit = VDD
    ADCON0bits.ADON = 0; // Turn on/off ADC module
    ADCON1bits.ADCS = 0b001; // FOSC/8 (2.0 ?s, 4 Mhz)
    //ADCS<2:0>: A/D Conversion Clock Select bits
    //000 = FOSC/2
    //001 = FOSC/8
    //010 = FOSC/32
    //x11 = FRC (clock derived from a dedicated internal oscillator = 500 kHz max)
    //100 = FOSC/4
    //101 = FOSC/16
    //110 = FOSC/64
#endif


}

void Timer0_Init(void) {
    //Timer0 Registers Prescaler= 4 - TMR0 Preset = 6 - Freq = 1000.00 Hz - Period = 0.001000 seconds
    OPTION_REGbits.T0CS = 0; // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0; // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0; // bit 3   Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS = 0b001; // bits 2-0  PS2:PS0: Prescaler Rate Select bits (1:4)

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

char UART_Init(const long int baudrate) {
    unsigned int x;
    x = (_XTAL_FREQ - baudrate * 64) / (baudrate * 64); //SPBRG for Low Baud Rate
    if (x > 255) //If High Baud Rage Required
    {
        x = (_XTAL_FREQ - baudrate * 16) / (baudrate * 16); //SPBRG for High Baud Rate
        BRGH = 1; //Setting High Baud Rate
    }
    if (x < 256) {
        //  TXSTAbits.
        SPBRG = x; //Writing SPBRG Register
        TXSTAbits.SYNC = 0; //Setting Asynchronous Mode, ie UART
        RCSTAbits.SPEN = 1; //Enables Serial Port
        TRISBbits.TRISB7 = 1; //As Prescribed in Datasheet
        TRISBbits.TRISB6 = 1; //As Prescribed in Datasheet
        RCSTAbits.CREN = 1; //Enables Continuous Reception
        TXSTAbits.TXEN = 1; //Enables Transmission
        return 1; //Returns 1 to indicate Successful Completion
    }
    return 0; //Returns 0 to indicate UART initialization failed


}

void UART_Write(char data) {
    while (!TRMT);
    TXREG = data;
}

void UART_Write_Next(char data) {
    TXREG = data;
}

void UART_Write2byte(int16_t data) {
    UART_Write((data) >> 8);
    UART_Write_Next((data));
}

char UART_TX_Empty() {
    return TRMT;
}

void UART_Write_Text(const char *text) {
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART_Write(text[i]);
}

char UART_Data_Ready() {
    return RCIF;
}

char UART_Read() {
    while (!RCIF);
    return RCREG;
}

void UART_Read_Text(char *Output, uint8_t length) {
    for (uint8_t i = 0; i < length; i++)
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

int32_t millis_32(void) {
    return t0_millis;
}

void VoltageDetector_Start(void) {
#ifdef useVoltageDetector
    ADCON0bits.ADON = 1; // Turn on/off ADC module
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    ADCON0bits.GO = 1; // Start a conversion
#endif    
}

void VoltageDetector_Check(void) {
    if (lowVoltageIs) {
        buzzer_duration=20;
        lowVoltageIs=0;
    }
}


