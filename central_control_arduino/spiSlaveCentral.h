#ifndef SPISLAVE_H
#define SPISLAVE_H

/*
 * INSTRUCTIONS FOR ADDING SPI OPERATIONS
 *
 * Make a #define with a unique uint8_t identifier for your operation in the
 * SPI COMMANDS section below in this header file. 
 *
 * Add a case to the switch statement in the "executeIncomingCommand" method
 * which contains the code to be executed upon receiving your command.
 *
 * If your operation requires sending data to the master:
 * ------------------------------------------------------
 * The code that you add to "executeIncomingCommand" must include a call to
 * stringBuffer->loadWithOutgoingData. This method will accept any data you
 * provide and send it to the master.
 *
 * If your operation requires receiving data from the master:
 * ----------------------------------------------------------
 * The code that you add to "executeIncomingCommand" must include a call to
 * stringBuffer->prepareForIncomingData, and must also set the "purposeForIncomingString"
 * member to your command's unique identifier.
 *
 * In addition, you must also add a case to the "executeReceivedString" method,
 * with code to be executed once the string has been received.
 **/

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
 *
 * If echo verification fails, the slave will send MASTER_ECHO_FAILED repeatedly
 * to indicate that the operation has failed. Master will then pull up slave select
 * and restart the operation.
 **/
 
#include <stdint.h>
#include "buffer.h"

// The largest expected data string must fit inside this buffer
#define STRING_BUFFER_SIZE              32

// Sent from the slave when output not required
#define EMPTY_BYTE                      0

#define NO_ERROR                        0

#define NO_INCOMING_STRING              0


/*
 * SLAVE SPI ERROR TYPES
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
 * SHUTDOWN ERROR CODES
 *
 * Used by the master to query if anything has gone wrong in hardware.
 **/
 
#define ALL_OK                              0
#define UNSAFE_ORIENTATION_SHUTDOWN         1


/*
 * SPI COMMANDS
 *
 * These imperatives are sent from the SPI master and
 * inform the slave how to behave.
 **/ 
#define RECIEVE_SHUTDOWN_CMD			0

#define LOAD_SHUTDOWN_STATUS		    100
#define LOAD_24V_READING		        101
#define LOAD_12V_READING                102
#define LOAD_ENERGY_CONSUMED			103


class SpiSlave {
    private:
        // Used to send an error message back to the master.
        uint8_t errorCondition;

        // Used to hold data when sending or receiving a string.
        Buffer *stringBuffer;

        // When a string is being read, this indicates what the string should do once received.
        // It is set by the master before sending the string.
        uint8_t purposeForIncomingString;
        
        // Set to false after the first transfer
        bool firstByte;
        
        void reset(void);
        void executeIncomingCommand(void);
        void executeReceivedString(void);
        uint8_t *getShutdownError(void);
        
    public:
        SpiSlave(void);
        ~SpiSlave();
        void update(void);
};
    

#endif
