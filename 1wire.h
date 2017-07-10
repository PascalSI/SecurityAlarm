//////////////////////////////////////////////////////////////////////////////// 
// 1wire.h                                                                    // 
//////////////////////////////////////////////////////////////////////////////// 
#ifndef _1wire_H 
#define _1wire_H 


//==============================  Includes  ==================================// 
//#include <p18f4580.h>  
//============================= Prototypes ===================================// 
void drive_one_wire_low (void); 
void drive_one_wire_high (void); 
unsigned char read__one_wire (void); 
void OW_write_bit (unsigned char write_data); 
unsigned char OW_read_bit (void); 
unsigned char OW_reset_pulse(void); 
void OW_write_byte (unsigned char write_data); 
unsigned char OW_read_byte (void); 
unsigned char calc_crc(unsigned char *mas);

#endif 
//=============================  End of 1wire.h  =============================// 