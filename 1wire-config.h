//////////////////////////////////////////////////////////////////////////////// 
// 1wire-Config.h                                                                    // 
//////////////////////////////////////////////////////////////////////////////// 

#ifndef _1WireConfig_H 
#define _1WireConfig_H 
//========================= Generic Definitions ==============================// 
#define        OW_HIGH        1 
#define        OW_LOW         0 
#define        OW_OUTPUT      TRIS_OUTPUT 
#define        OW_INPUT       TRIS_INPUT 
#define        OW_SET         1 
#define        OW_CLEAR       0 
//======================== 1-Wire Port Pin Definition ========================// 
// This Configuration is required to make any PIC MicroController 
// I/O pin as Open drain to drive 1-wire. 
//============================================================================// 
#define OW_PIN_DIRECTION         DOOR_BUTTON_TRIS           //7_bit 
#define OW_WRITE_PIN             DOOR_BUTTON_WRITE    
#define OW_READ_PIN              DOOR_BUTTON 
#define OW_FLUSH_PIN             DOOR_BUTTON_FLUSH_PORT 
#endif   
//============================= End of Config.h  =============================// 