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
 * If slave is not ready to receive a byte, then it will output
 * NOT_READY to the master.
 *
 * To set data on the Arduino slave:
 *
 * MOSI                                |   MISO
 * ------------------------------------|--------------
 * RECEIVE_A_ORIENTATION               |   EMPTY_BYTE               (slave initialises buffer by calling prepareForIncomingData())
 * ASCII CHAR 1                        |   RECEIVE_A_ORIENTATION    (slave echoes previous messages for master verification)
 * ASCII CHAR 2                        |   ASCII CHAR 1
 * ASCII CHAR 3                        |   ASCII CHAR 2
 * ASCII CHAR 4                        |   ASCII CHAR 3
 *                                     |
 *                 ...                 |      ...
 *                                     |
 * NUL CHAR                            |   ASCII CHAR N             (slave calls Actuator.setDesiredOrientation with data from buffer)
 * NUL CHAR                            |   NUL CHAR
 *
 * If echo verification fails, the master will pull up slave select
 * and restart the operation.
 *
 *
 *
 *
 * To receive data from the Arduino slave:
 *
 * MOSI                                |   MISO
 * ------------------------------------|--------------
 * LOAD_A_CONTROLLER_OUTPUT            |   EMPTY_BYTE               (slave loads data onto buffer)
 * EMPTY_BYTE                          |   ASCII CHAR 1
 * ASCII CHAR 1                        |   ASCII CHAR 2
 * ASCII CHAR 2                        |   ASCII CHAR 3
 *                                     |
 *                 ...                 |      ...
 *                                     |
 * ASCII CHAR N                        |   NUL CHAR
 * NUL CHAR                            |   NUL CHAR
 *
 * If echo verification fails, the slave will send MASTER_ECHO_FAILED repeatedly
 * to indicate that the operation has failed. Master will then pull up slave select
 * and restart the operation.
 **/
 
#include <stdint.h>
#include "actuator.h"
#include "hubMotor.h"
#include "buffer.h"

// The largest expected data string must fit inside this buffer
#define STRING_BUFFER_SIZE              32

// Sent from the slave when output not required
#define EMPTY_BYTE                      0

#define NO_ERROR                        0


/*
 * SLAVE ERROR TYPES
 *
 * These are sent to the master to indicate an error. Any strings sent from the slave cannot
 * contain any of these characters.
 **/

#define FORBIDDEN_RANGE_START               252
 
// The outgoing message contained an ASCII character equal to one of the following error codes.
#define MESSAGE_HAS_FORBIDDEN_CHARS         252
 
// The slave did not understand the command byte from the master.
#define COMMAND_NOT_RECOGNISED              253

// If slave is not ready to receive a byte, then it will output NOT_READY to the master.
#define NOT_READY                           254

// When strings are being sent to the master, and the master echo is incorrect, this byte is sent to indicate
// that the operation has failed.
#define MASTER_ECHO_FAILED                  255


/*
 * SPI COMMANDS
 *
 * These imperatives are sent from the SPI master and
 * inform the slave how to behave.
 **/ 
#define RECEIVE_A_ORIENTATION           0
#define RECEIVE_HM_STATE                1
#define RECEIVE_HM_THROTTLE             2
 
#define LOAD_A_MEASURED_ORIENTATION     100
#define LOAD_A_CONTROLLER_OUTPUT        101


class SpiSlave {
    private:
        Actuator *actuator;
        HubMotor *hubMotor;
        
        void processIncomingCommand(uint8_t incomingByte);
        void processIncomingAscii(char incomingByte);
        
    public:
        SpiSlave(void);
        void update(void);
}
    

#endif
