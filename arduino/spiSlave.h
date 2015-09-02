#ifndef SPISLAVE_H
#define SPISLAVE_H

/*
 * DESCRIPTION OF COMMUNICATION PROTOCOL
 *
 * At the start of each operation the master pulls slave select low,
 * and at the end of each operation the master sets the slave select
 * high.
 *
 * In the tables below, the master outputs a byte on MOSI and then the
 * slave outputs a byte on MISO. The slave never goes first.
 *
 * To set data on the Arduino slave:
 *
 * MOSI                                |   MISO
 * ------------------------------------|--------------
 * RECEIVE_A_ORIENTATION               |   EMPTY_BYTE      (slave initialises buffer)
 * ASCII BYTE                          |   EMPTY_BYTE
 * ASCII BYTE                          |   EMPTY_BYTE
 * ASCII BYTE                          |   EMPTY_BYTE
 *
 *                 ...
 *
 * ETX_CHAR                            |   EMPTY_BYTE      (slave calls Actuator.setDesiredOrientation with data from buffer)
 *
 *
 *
 * To receive data from the Arduino slave:
 *
 * MOSI                                |   MISO
 * ------------------------------------|--------------
 * LOAD_A_CONTROLLER_OUTPUT            |   EMPTY_BYTE      (slave loads data onto buffer)
 * SEND_DATA                           |   ASCII BYTE
 * SEND_DATA                           |   ASCII BYTE
 * SEND_DATA                           |   ASCII BYTE
 *
 *                 ...
 *
 * SEND_DATA                           |   ETX_CHAR
 **/
 
#include <stdint.h>
#include "actuator.h"
#include "hubMotor.h"

// The largest expected data string must fit inside this buffer
#define STRING_BUFFER_SIZE              32

// This is used to indicate the end of an ASCII data string (end of text)
#define ASCII_ETX                       3

// Sent from the slave when output not required
#define EMPTY_BYTE                      0

// Unless the master has indicated that it is sending ASCII, this will be stored in purposeForIncomingString
#define NOT_RECEIVING_ASCII             0

#define BASE_TEN                        10


/*
 * SPI COMMANDS
 *
 * These imperatives are sent from the SPI master and
 * inform the slave how to behave.
 **/ 
#define SEND_DATA                       0 

#define RECEIVE_A_ORIENTATION           1
#define RECEIVE_HM_STATE                2
#define RECEIVE_HM_THROTTLE             3
 
#define LOAD_A_MEASURED_ORIENTATION     100
#define LOAD_A_CONTROLLER_OUTPUT        101


class SpiSlave {
private:
    volatile uint8_t purposeForIncomingString;
    volatile char *stringBuffer;
    volatile uint8_t stringBufferIndex;
    volatile bool stringBufferReady;
    Actuator *actuator;
    HubMotor *hubMotor;
    
    void retrieveIncomingByte(void);
    void processIncomingCommand(uint8_t incomingByte);
    void processIncomingAscii(char incomingByte);    
    void resetBuffer(void);
    void sendCharFromBuffer(void);
    
public:
    SpiSlave(void);
    ~SpiSlave(void);
    bool stringToUint16(const char *string, uint16_t *result);
}
    

#endif