/**
 * @file keypad.h
 * @author Embedded Laboratory
 * @date June 16, 2016
 * @brief Matrix Keypad Related Macros and Function Prototypes.
 *
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#ifndef __stdint_are_defined
#define __stdint_are_defined
#include <stdint.h>        /* For uint8_t definition */
#endif

//#include "user.h"
//#include "config.h"

#define MAX_ROW         3                 /**< Maximum Row.*/
#define MAX_COL         3                 /**< Maximum Column.*/

#define ROW_1_PIN       KEYPAD_ROW1     /**< Row 1 Pin Number.*/
#define ROW_1_DIR       KEYPAD_ROW1_TRIS  /**< Row 1 Direction.*/
#define ROW_2_PIN       KEYPAD_ROW2     /**< Row 2 Pin Number.*/
#define ROW_2_DIR       KEYPAD_ROW2_TRIS  /**< Row 2 Direction.*/
#define ROW_3_PIN       KEYPAD_ROW3     /**< Row 3 Pin Number.*/
#define ROW_3_DIR       KEYPAD_ROW3_TRIS  /**< Row 3 Direction.*/
//#define ROW_4_PIN       PORTBbits.RB3     /**< Row 4 Pin Number.*/
//#define ROW_4_DIR       TRISBbits.TRISB3  /**< Row 5 Direction.*/

#define COL_1_PIN       KEYPAD_COL1     /**< Col 1 Pin Number.*/
#define COL_1_DIR       KEYPAD_COL1_TRIS  /**< Col 1 Direction.*/
#define COL_2_PIN       KEYPAD_COL2     /**< Col 2 Pin Number.*/
#define COL_2_DIR       KEYPAD_COL2_TRIS  /**< Col 2 Direction.*/
#define COL_3_PIN       KEYPAD_COL3     /**< Col 3 Pin Number.*/
#define COL_3_DIR       KEYPAD_COL3_TRIS  /**< Col 3 Direction.*/
//#define COL_4_PIN       PORTBbits.RB7     /**< Col 4 Pin Number.*/
//#define COL_4_DIR       TRISBbits.TRISB7  /**< Col 4 Direction.*/

#define KEYPAD_DEBOUNCE_TIME    20u       /**< Debounce Time in msec.*/
#define KEYPAD_HOLD_TIME        2000u     /**< Keypad Hold Time before Repeat.*/
#define KEYPAD_REPEAT_TIME      100u      /**< Keypad Repeat Time.*/

#define NO_KEYs                 255u      /**< No Key Pressed.*/
#define NO_KEY                  0u        /**< No Key Pressed.*/

/**
 * @brief Keypad States
 *
 * Different States of Matrix Keypad used by State Machine.
 */
typedef enum _Keypad_State_e
{
  KEYPAD_UP = 0,      /**< Key Up State, Initial State.*/
  KEYPAD_PRESSED,     /**< Key Pressed Detected State.*/
  KEYPAD_DOWN,        /**< Key Still Pressed State.*/
  KEYPAD_HELD,        /**< Key Hold Down for Repetetion State.*/
  KEYPAD_RELEASED,    /**< Key Release State.*/
  KEYPAD_DEBOUNCE     /**< Key Debouncing State.*/
} Keypad_State_e;

/**
 * @brief Keypad Structure
 *
 * Handle Matrix Keypad Data.
 */
typedef struct _Keypad_s
{
  uint8_t keyPressed;             /**< Key Pressed Detected.*/
  uint8_t keySensed;              /**< Key Sensed based on algorithm.*/
  uint16_t keyStatus_timeStamp;    /**< Key State Change Timestamp.*/
  Keypad_State_e keypad_state;  /**< Keypad Current State.*/
} Keypad_s;

/* Public Function Prototypes*/
void Initialize_Keypad( void );
uint8_t getKey( void );

#endif /* KEYPAD_H_ */
