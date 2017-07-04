#include "pc2keyboard.h"

void PC2Keyboard_Init(void) {
    //PC2 CLOCK - KEYPAD_COL3 (INT)RA2/INT
    //INTEDG
    CM2CON0 = 0;
    ANSELbits.ANS2 = 0; //0 = Digital I/O. Pin is assigned to port or special function.
    OPTION_REGbits.INTEDG = 0; //falling egde
    INTCONbits.INTE = 1; //enable inetrupt by INT 
    INTCONbits.INTF = 0;

}

void PC2Keboard_Process() {
    if (pc2kbd_ready == 1) {
        char kbd_char = 0x0;
        switch (pc2kbd_answer) {
            case 0x1c:
                kbd_char = 'A';
                break;
            case 0x32:
                kbd_char = 'B';
                break;
            case 0x21:
                kbd_char = 'C';
                break;
            case 0x23:
                kbd_char = 'D';
                break;
            case 0x24:
                kbd_char = 'E';
                break;
            case 0x2B:
                kbd_char = 'F';
                break;
            case 0x34:
                kbd_char = 'G';
                break;
            case 0x33:
                kbd_char = 'H';
                break;
            case 0x43:
                kbd_char = 'I';
                break;
            case 0x3B:
                kbd_char = 'J';
                break;
            case 0x42:
                kbd_char = 'K';
                break;
            case 0x4B:
                kbd_char = 'L';
                break;
            case 0x3A:
                kbd_char = 'M';
                break;
            case 0x31:
                kbd_char = 'N';
                break;
            case 0x44:
                kbd_char = 'O';
                break;
            case 0x4D:
                kbd_char = 'P';
                break;
            case 0x15:
                kbd_char = 'Q';
                break;
            case 0x2D:
                kbd_char = 'R';
                break;
            case 0x1B:
                kbd_char = 'S';
                break;
            case 0x2C:
                kbd_char = 'T';
                break;
            case 0x3C:
                kbd_char = 'U';
                break;
            case 0x2A:
                kbd_char = 'V';
                break;
            case 0x1D:
                kbd_char = 'W';
                break;
            case 0x22:
                kbd_char = 'X';
                break;
            case 0x35:
                kbd_char = 'Y';
                break;
            case 0x1A:
                kbd_char = 'Z';
                break;
            case 0x66:
                //_clear;
                break;
            case 0x5A:
                kbd_char = 'e'; //"Enter"
                break;
            case 0x45:
                kbd_char = '0';
                break;
            case 0x16:
                kbd_char = '1';
                break;
            case 0x1E:
                kbd_char = '2';
                break;
            case 0x26:
                kbd_char = '3';
                break;
            case 0x25:
                kbd_char = '4';
                break;
            case 0x2E:
                kbd_char = '5';
                break;
            case 0x36:
                kbd_char = '6';
                break;
            case 0x3D:
                kbd_char = '7';
                break;
            case 0x3E:
                kbd_char = '8';
                break;
            case 0x46:
                kbd_char = '9';
                break;
                //case 0xff:
                //':'; 
                //break;
        }
//       if (kbd_char) {
//#ifdef useDebugRS232    
//            UART_Write(kbd_char);
//#endif
////        } else {
#ifdef useDebugRS232    
            UART_Write(pc2kbd_answer);
#endif 
//       }
        pc2kbd_answer = 0;
        pc2kbd_ready = 0;
    }
}
