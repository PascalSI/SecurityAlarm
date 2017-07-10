#include "pc2keyboard.h"

void PC2Keyboard_Init(void) {
    //PC2 CLOCK - KEYPAD_COL3 (INT)RA2/INT
    //INTEDG
    CM2CON0 = 0;
    ANSELbits.ANS2 = 0; //0 = Digital I/O. Pin is assigned to port or special function.
    OPTION_REGbits.INTEDG = 0; //falling egde
    INTCONbits.INTE = 1; //enable inetrupt by INT 
    INTCONbits.INTF = 0;
   
    INTCONbits.RABIE = 0;
    IOCAbits.IOCA2 = 0;

}

void PC2Keboard_Process() {
    if (pc2kbd_ready == 1) {
        char kbd_char = 0x0;
        switch (pc2kbd_answer) {
            case 0x70:
                kbd_char = '0';
                break;
            case 0x69:
                kbd_char = '1';
                break;
            case 0x72:
                kbd_char = '2';
                break;
            case 0x7A:
                kbd_char = '3';
                break;
            case 0x6B:
                kbd_char = '4';
                break;
            case 0x73:
                kbd_char = '5';
                break;
            case 0x74:
                kbd_char = '6';
                break;
            case 0x6C:
                kbd_char = '7';
                break;
            case 0x75:
                kbd_char = '8';
                break;
            case 0x7D:
                kbd_char = '9';
                break;
        }
       if (kbd_char) {
#ifdef useDebugRS232    
            UART_Write(kbd_char);
#endif
////        } else {
//#ifdef useDebugRS232    
//            UART_Write(pc2kbd_answer);
//#endif 
       }
        pc2kbd_answer = 0;
        pc2kbd_ready = 0;
    }
}
