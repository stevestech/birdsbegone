// Please read the usage limitations in buffer.h

#include <string.h>
#include <stdio.h>

#include "buffer.h"

Buffer::Buffer(uint8_t size) {
    buffer = new char[size];
    index = 0;
    
    // This flag determines if bytes can be read from the buffer.
    sending = false;
    
    // This flag determines if bytes can be written to the buffer.
    receiving = false;
    
    // This flag indicates if the incoming string has been nul terminated yet.
    receivingComplete = false;
}


Buffer::~Buffer(void) {
    delete buffer;
}


// Reset the index and all flags.
// This function is not permitted to be called from outside of
// the PCINT0 interrupt service routine.
void Buffer::reset(void) {
    index = 0;
    sending = false;
    receiving = false;
    receivingComplete = false;
}


// The load methods will accept a string, double, uint16_t or uint8_t,
// convert numeric values into a string and load it onto the buffer.
void Buffer::loadWithOutgoingData(char *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        // Similar to strcpy, but respects the buffer's length
        snprintf(buffer, sizeof(buffer), "%s", data);
    }
}


void Buffer::loadWithOutgoingData(double *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        snprintf(buffer, sizeof(buffer), "%f", *data);
    }
}


void Buffer::loadWithOutgoingData(uint16_t *data) {
    if (!receiving) {    
        index = 0;
        sending = true;
        snprintf(buffer, sizeof(buffer), "%d", *data);
    }
}


void Buffer::loadWithOutgoingData(uint8_t *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        snprintf(buffer, sizeof(buffer), "%d", *data);
    }
}


// This function is not permitted to be called from outside of
// the SPI_STC interrupt service routine.
char Buffer::popNextByte(void) {
    // If data has been loaded onto the buffer using load(), then
    // return next byte until the index reaches the end of the buffer
    if ((sending) && (index < sizeof(buffer))) {
        return buffer[index++];
    }
    
    return ASCII_NUL;
}


char Buffer::getPrevByte(void) {
    if ((sending) &&
        (index - 1 >= 0) &&
        (index - 1 < sizeof(buffer))) {
            
        return buffer[index - 1];
    }
    
    return ASCII_NUL;
}


void Buffer::prepareForIncomingData(void) {
    if (!sending) {
        receiving = true;
    }
}


void Buffer::appendByte(char *newByte) {
    // Append newValue to the buffer if there is room for it, and we haven't
    // already received the nul terminator.
    if ((receiving) &&
        (!receivingComplete) &&
        (index < sizeof(buffer))) {
            
        buffer[index++] = *newByte;
    }
    
    // Master has sent the nul terminator, so don't receive any more bytes.
    if (*newByte == ASCII_NUL) {
        receivingComplete = true;
    }
}


bool Buffer::isReceivingComplete(void) {
    return receiving && receivingComplete;
}


bool Buffer::isReceiving(void) {
    return receiving;
}


bool Buffer::isSending(void) {
    return sending;
}
