// Please read the usage limitations in buffer.h
#include <string.h>
#include <stdio.h>

#include "buffer.h"

Buffer::Buffer(uint8_t size) {
    buffer = new char[size];
    bufferLength = size;
    reset();
}


Buffer::~Buffer(void) {
    delete buffer;
}


// Reset the index and all flags.
void Buffer::reset(void) {
    index = 0;
    sending = false;
    receiving = false;
    sendingComplete = false;
    receivingComplete = false;
}


// The load methods will accept a string, double, uint16_t or uint8_t,
// convert numeric values into a string and load it onto the buffer.
void Buffer::loadWithOutgoingData(char *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        // Similar to strcpy, but respects the buffer's length
        snprintf(buffer, bufferLength, "%s", data);
    }
}


void Buffer::loadWithOutgoingData(double *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        snprintf(buffer, bufferLength, "%d", (int32_t) *data);
    }
}


void Buffer::loadWithOutgoingData(uint16_t *data) {
    if (!receiving) {    
        index = 0;
        sending = true;
        snprintf(buffer, bufferLength, "%d", *data);
    }
}


void Buffer::loadWithOutgoingData(uint8_t *data) {
    if (!receiving) {
        index = 0;
        sending = true;
        snprintf(buffer, bufferLength, "%d", *data);
    }
}


char Buffer::popByte(void) {
    // If data has been loaded onto the buffer using load(), then
    // return next byte until the index reaches the end of the buffer
    if ((sending) &&
        (!sendingComplete) &&
        (index < bufferLength)) {
            
        if (buffer[index] == ASCII_NUL) {
            sendingComplete = true;
        }
        
        return buffer[index++];
    }
    
    return ASCII_NUL;
}


void Buffer::prepareForIncomingData(void) {
    if (!sending) {
        receiving = true;
    }
}


void Buffer::appendByte(char newByte) {
    // Append newValue to the buffer if there is room for it, and we haven't
    // already received the nul terminator.
    if ((receiving) &&
        (!receivingComplete) &&
        (index < bufferLength)) {
            
        // Master has sent the nul terminator, so don't receive any more bytes.
        if (newByte == ASCII_NUL) {
            receivingComplete = true;
        }
    
        buffer[index++] = newByte;
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
