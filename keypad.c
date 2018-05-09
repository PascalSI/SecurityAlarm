/**
 * @file keypad.c
 * @author Embedded Laboratory
 * @date June 16, 2016
 * @brief Keypad Sense related functions and Algorithm.
 * 
 */

#include "keypad.h"
#include "user.h"

static Keypad_s s_keypad; /**< Keypad Structure.*/
static uint8_t KeyPressTable[MAX_ROW][MAX_COL] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
#ifdef ROW_4_PIN   
    '*', '0', '#'
#endif            
}; /**< Key Look-Up Table.*/

//  '1','2','3','A',
//  '4','5','6','B',
//  '7','8','9','C',
//  '*','0','#','D'

/* Private Functions */
static uint8_t _Process_Keypress(void);
static uint8_t _Sense_Keypress(void);

/**
 * @brief Initialize Matrix Keyboard.
 *
 * Initialize 4x4 Matrix Keypad.
 * 
 */
void Initialize_Keypad(void) {
    // Initialize Rows as Output
    ROW_1_DIR = TRIS_OUTPUT;
    ROW_2_DIR = TRIS_OUTPUT;
    ROW_3_DIR = TRIS_OUTPUT;
#ifdef ROW_4_DIR    
    ROW_4_DIR = TRIS_OUTPUT;
#endif    
    ROW_1_PIN = KEYPAD_ROW1_OFF;
    ROW_2_PIN = KEYPAD_ROW2_OFF;
    ROW_3_PIN = KEYPAD_ROW3_OFF;
#ifdef ROW_4_PIN    
    ROW_4_PIN = KEYPAD_ROW4_OFF;
#endif    

    // Enable Pull-Up of Columns or place hardware pull-ups

    // Initialize Columns as Input
    COL_1_DIR = TRIS_INPUT;
    COL_2_DIR = TRIS_INPUT;
    COL_3_DIR = TRIS_INPUT;
    //COL_4_DIR = 1;
}

/**
 * @brief Get Pressed Key Value.
 *
 * This function returns the key pressed on the Matrix Keypad.
 * return Pressed Key Value Stored in Config.
 * @note This function returns 0u/NO_KEY if no key press is detected.
 */
uint8_t getKey(void) {
    uint8_t key;
    key = _Process_Keypress();
    if (key == NO_KEYs) {
        key = NO_KEY;
    }
    return key;
}

/**
 * @brief Scan Key Press.
 *
 * This is a private function, this returns the pressed key, but it don't care
 * about anything else, like debouncing and other things.
 * return Pressed Key Value Stored in Config.
 * @note This function returns 0xff/NO_KEYs if no key press is detected.
 */

static bit SaveStatepROW_3_PIN;
#ifdef ROW_4_PIN    
static bit SaveStatepROW_4_PIN;
#endif

static uint8_t _Sense_Keypress(void) {
    uint8_t row = 0u, col = 0u;
    uint8_t keypress = NO_KEYs;
    SaveStatepROW_3_PIN = ROW_3_PIN;
#ifdef ROW_4_PIN        
    SaveStatepROW_4_PIN = ROW_4_PIN;
#endif    

    // Reset Row Values
    ROW_1_PIN = KEYPAD_ROW1_ON;
    ROW_2_PIN = KEYPAD_ROW2_ON;
    ROW_3_PIN = KEYPAD_ROW3_ON;
#ifdef ROW_4_PIN        
    ROW_4_PIN = KEYPAD_ROW4_ON;
#endif    
    //    ROW_4_PIN = 0;

    KEYPAD_ROWS_FLUSH_PORT;
    NOP();

    if (COL_1_PIN == KEYPAD_COL1_ON){
        col = 1u;
    }else if (COL_2_PIN == KEYPAD_COL2_ON){
        col = 2u;
    }else if (COL_3_PIN == KEYPAD_COL3_ON){
        col = 3u;
    }
    //  if( !COL_4_PIN )
    //    col = 4u;

    // If Some Key is Pressed
    if (col) {
        for (row = 1u; row <= MAX_ROW; row++) {
            switch (row) {
                case 1:
                    // Ground Row-1
                    ROW_1_PIN = KEYPAD_ROW1_ON;
                    ROW_2_PIN = KEYPAD_ROW2_OFF;
                    ROW_3_PIN = KEYPAD_ROW3_OFF;
                    ROW_4_PIN = KEYPAD_ROW4_OFF;
                    break;
                case 2:
                    // Ground Row-2
                    ROW_1_PIN = KEYPAD_ROW1_OFF;
                    ROW_2_PIN = KEYPAD_ROW2_ON;
                    ROW_3_PIN = KEYPAD_ROW3_OFF;
                    ROW_4_PIN = KEYPAD_ROW4_OFF;                    
                    break;
                case 3:
                    // Ground Row-3
                    ROW_1_PIN = KEYPAD_ROW1_OFF;
                    ROW_2_PIN = KEYPAD_ROW2_OFF;
                    ROW_3_PIN = KEYPAD_ROW3_ON;
                    ROW_4_PIN = KEYPAD_ROW4_OFF;                    
                    break;
                case 4:
                    // Ground Row-4
                    ROW_1_PIN = KEYPAD_ROW1_OFF;
                    ROW_2_PIN = KEYPAD_ROW2_OFF;
                    ROW_3_PIN = KEYPAD_ROW3_OFF;
                    ROW_4_PIN = KEYPAD_ROW4_ON;                                        
                    break;
            }
            KEYPAD_ROWS_FLUSH_PORT;
            // Scan Column
            if (COL_1_PIN == KEYPAD_COL1_ON)
                break;
            if (COL_2_PIN == KEYPAD_COL2_ON)
                break;
            if (COL_3_PIN == KEYPAD_COL3_ON)
                break;
            //      if( !COL_4_PIN )
            //        break;
        }

        //restore state of shared pin
        ROW_3_PIN = SaveStatepROW_3_PIN;
#ifdef ROW_4_PIN            
        ROW_4_PIN = SaveStatepROW_4_PIN;        
#endif        
        KEYPAD_ROWS_FLUSH_PORT;

        if (row && col) {
            if (row <= MAX_ROW && col <= MAX_COL)
                keypress = KeyPressTable[row - 1][col - 1];
        }
    }
    return keypress;
}

/**
 * @brief Process Detected Key Press.
 *
 * This is a private function, it process the detected key press, anc checks its
 * validity.
 * return Pressed Key Value Stored in Config.
 * @note This function returns 0xff/NO_KEYs if no key press is detected.
 */
static uint8_t _Process_Keypress(void) {
    s_keypad.keyPressed = _Sense_Keypress();
    switch (s_keypad.keypad_state) {
        case KEYPAD_UP:
            if (s_keypad.keyPressed != NO_KEYs) {
                s_keypad.keySensed = s_keypad.keyPressed;
                s_keypad.keyStatus_timeStamp = millis();
                s_keypad.keypad_state = KEYPAD_DEBOUNCE;
            } else {
                s_keypad.keySensed = NO_KEYs;
            }
            break;
        case KEYPAD_DEBOUNCE:
            if (s_keypad.keyPressed != NO_KEYs) {
                if (s_keypad.keyPressed == s_keypad.keySensed) {
                    if ((millis() - s_keypad.keyStatus_timeStamp) > KEYPAD_DEBOUNCE_TIME) {
                        s_keypad.keypad_state = KEYPAD_PRESSED;
                        s_keypad.keyStatus_timeStamp = millis();
                        return s_keypad.keySensed;
                    }
                } else {
                    s_keypad.keyStatus_timeStamp = millis();
                    s_keypad.keySensed = s_keypad.keyPressed;
                }
            } else {
                s_keypad.keypad_state = KEYPAD_UP;
                s_keypad.keySensed = NO_KEYs;
            }
            break;
        case KEYPAD_PRESSED:
            if (s_keypad.keyPressed != NO_KEYs) {
                if (s_keypad.keySensed == s_keypad.keyPressed) {
                    s_keypad.keypad_state = KEYPAD_DOWN;
                } else {
                    s_keypad.keypad_state = KEYPAD_DEBOUNCE;
                }
            } else {
                s_keypad.keypad_state = KEYPAD_RELEASED;
            }
            s_keypad.keyStatus_timeStamp = millis();
            break;
        case KEYPAD_DOWN:
            if (s_keypad.keySensed == s_keypad.keyPressed) {
                if ((millis() - s_keypad.keyStatus_timeStamp) > KEYPAD_HOLD_TIME) {
                    s_keypad.keypad_state = KEYPAD_HELD;
                    s_keypad.keyStatus_timeStamp = millis();
                }
            } else {
                s_keypad.keypad_state = KEYPAD_RELEASED;
                s_keypad.keyStatus_timeStamp = millis();
            }
            break;
        case KEYPAD_HELD:
            if (s_keypad.keySensed != s_keypad.keyPressed) {
                s_keypad.keypad_state = KEYPAD_RELEASED;
                s_keypad.keyStatus_timeStamp = millis();
                return s_keypad.keySensed;
            } else if (millis() - s_keypad.keyStatus_timeStamp > KEYPAD_REPEAT_TIME) {
                s_keypad.keyStatus_timeStamp = millis();
                return s_keypad.keySensed;
            }
            break;
        case KEYPAD_RELEASED:
            if (s_keypad.keyPressed == NO_KEYs) {
                s_keypad.keypad_state = KEYPAD_UP;
            } else {
                s_keypad.keypad_state = KEYPAD_DEBOUNCE;
                s_keypad.keySensed = s_keypad.keyPressed;
                s_keypad.keyStatus_timeStamp = millis();
            }
            break;
        default:
            s_keypad.keypad_state = KEYPAD_UP;
            s_keypad.keyPressed = 0u;
            s_keypad.keySensed = 0u;
            s_keypad.keyStatus_timeStamp = 0u;
            break;
    }
    return NO_KEYs;
}
