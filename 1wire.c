////////////////////////////////////////////////////////////////////////////////
//                 1-WIRE COMMUNICATION PROTOCOL    DS1990?                      //
// 1wire.c                                                                    //
////////////////////////////////////////////////////////////////////////////////


//============================== Includes ====================================//
#include "1wire-config.h"
#include "1wire.h"
#include "system.h"
#include "user.h"


//============================= Variables ====================================//

//unsigned char macro_delay;

//----------------------------------------------------------------------------//
// Function:        void drive_OW_low (void)
// PreCondition:    None
// Input:           None
// Output:          None
// Overview:        Configure the OW_PIN as Output and drive the OW_PIN LOW.
//----------------------------------------------------------------------------//

void drive_OW_low(void) {
    OW_WRITE_PIN = OW_LOW;
    OW_PIN_DIRECTION = OW_OUTPUT;
    //OW_READ_PIN  = OW_LOW;      
    OW_FLUSH_PIN;
}

//----------------------------------------------------------------------------//
// Function:        void drive_OW_high (void)
// PreCondition:    None
// Input:            None
// Output:           None
// Overview:         Configure the OW_PIN as Output and drive the OW_PIN HIGH.
//----------------------------------------------------------------------------//

void drive_OW_high(void) {
    OW_PIN_DIRECTION = OW_INPUT;
    //OW_READ_PIN = OW_HIGH;        
    OW_WRITE_PIN = OW_HIGH;
    //OW_FLUSH_PIN;


}

//----------------------------------------------------------------------------//
// Function:        unsigned char read_OW (void)
// PreCondition:    None
// Input:           None
// Output:          Return the status of OW pin.
// Overview:        Configure as Input pin and Read the status of OW_PIN
//----------------------------------------------------------------------------//

unsigned char read_OW(void) {
    unsigned char read_data = 0;

    //OW_WRITE_PIN = INPUT;
    OW_PIN_DIRECTION = OW_INPUT;

    if (OW_HIGH == OW_READ_PIN)
        read_data = OW_SET;
    else
        read_data = OW_CLEAR;

    return read_data; //(OW_READ_PIN == OW_HIGH)? OW_SET : OW_CLEAR;
}

//----------------------------------------------------------------------------//
// Function:        unsigned char OW_reset_pulse(void)
// PreCondition:    None
// Input:           None
// Output:          Return the Presense Pulse from the slave.
// Overview:        Initialization sequence start with reset pulse.
//                 This code generates reset sequence as per the protocol
//----------------------------------------------------------------------------//
#define _tunetime_ 0.95

unsigned char OW_reset_pulse(void) {
    unsigned char presence_detect;

    if (read_OW() == 0) {
        return 1; // error
    }

    drive_OW_low(); // Drive the bus low

    CLRWDT();
    __delay_us(480u * _tunetime_); // delay 480 microsecond (us)

    drive_OW_high(); // Release the bus
    CLRWDT();
    __delay_us(70u * _tunetime_); // delay 70 microsecond (us)

    presence_detect = read_OW(); //Sample for presence pulse from slave
    CLRWDT();
    __delay_us(410u * _tunetime_); // delay 410 microsecond (us)

    if (read_OW() == 0) {
        drive_OW_high();
        return 1; // error
    };
    drive_OW_high(); // Release the bus

    return presence_detect;
}

//----------------------------------------------------------------------------//
// Function:       void OW_write_bit (unsigned char write_data)
// PreCondition:    None
// Input:          Write a bit to 1-wire slave device.
// Output:          None
// Overview:        This function used to transmit a single bit to slave device.
//
//----------------------------------------------------------------------------//

void OW_write_bit(unsigned char write_bit) {
    di();
    if (write_bit) {
        //writing a bit '1'
        drive_OW_low();
        __delay_us(6); // Drive the bus low
        // delay 6 microsecond (us)

        drive_OW_high();
        __delay_us(64); // Release the bus
        // delay 64 microsecond (us)
    } else {
        //writing a bit '0'
        drive_OW_low(); // Drive the bus low
        __delay_us(60); // delay 60 microsecond (us)
        drive_OW_high();
        // Release the bus
        __delay_us(10);
        // delay 10 microsecond for recovery (us)
    }
    ei();
}

//----------------------------------------------------------------------------//
// Function:        unsigned char OW_read_bit (void)
// PreCondition:    None
// Input:           None
// Output:          Return the status of the OW PIN
// Overview:        This function used to read a single bit from the slave device.
//
//----------------------------------------------------------------------------//

unsigned char OW_read_bit(void) {
    di();
    unsigned char read_data; //reading a bit
    drive_OW_low(); // Drive the bus low
    __delay_us(6); // delay 6 microsecond (us)


    drive_OW_high(); // Release the bus
    __delay_us(9); // delay 9 microsecond (us)


    read_data = read_OW(); //Read the status of OW_PIN
    __delay_us(55); // delay 55 microsecond (us)
    ei();
    return read_data;
}

//----------------------------------------------------------------------------//
// Function:        void OW_write_byte (unsigned char write_data)
// PreCondition:    None
// Input:           Send byte to 1-wire slave device
// Output:          None
// Overview:        This function used to transmit a complete byte to slave device.
//
//----------------------------------------------------------------------------//

void OW_write_byte(unsigned char write_data) {
    unsigned char i;

    for (i = 0; i < 8; i++) {
        OW_write_bit(write_data & 0x01); //Sending LS-bit first
        write_data >>= 1; // shift the data byte for the next bit to send

    }
}

//----------------------------------------------------------------------------//
// Function:        unsigned char OW_read_byte (void)
// PreCondition:    None
// Input:           None
// Output:          Return the read byte from slave device
// Overview:        This function used to read a complete byte from the slave device.
//
//----------------------------------------------------------------------------//

unsigned char OW_read_byte(void) {
    unsigned char i, result = 0;

    for (i = 0; i < 8; i++) {
        result >>= 1; // shift the result to get it ready for the next bit to receive
        if (OW_read_bit()) {
            result |= 0x80; // if result is one, then set MS-bit
        }


    }
    return result;
}

/*
 Calculate CRC iButton
 
 */


unsigned char calc_crc(unsigned char *mas) {
    unsigned char i, data, crc, fb, st_byt;
    st_byt = 0;
    crc = 0;
    do {
        data = mas[st_byt];
        for (i = 0; i < 8; i++)// bits in byte
        {
            fb = crc ^ data;
            fb &= 1;
            crc >>= 1;
            data >>= 1;
            if (fb == 1) crc ^= 0x8c; // polinom iButton
        }
        st_byt++;
    } while (st_byt != 7); //bytes in array mas
    return crc;
}
//============================== End of 1wire.c ==============================//