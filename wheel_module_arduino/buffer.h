#ifndef BUFFER_H
#define BUFFER_H

/*
 * LIMITATIONS
 *
 * The string.h library that comes with Arduino does not support string formatting
 * with floating point data types, such as double. Therefore only integer data types
 * are accepted by loadWithOutgoingData().
 *
 * You can either write to, or read from this buffer, but not both
 * simultaneously. The receiving and sending flags indicate which mode
 * the buffer is currently operating in.
 *
 * When writing to the buffer these functions are called:
 *
 * prepareForIncomingData()
 * appendByte(newByte)                      <- called repeatedly
 * buffer is read directly to retrieve completed string
 *
 * When reading from the buffer these functions are called:
 *
 * loadWithOutgoingData(data)               <- fills the buffer with a string
 * popNextByte()                            <- called repeatedly
 **/

#include <stdint.h>

#ifndef ASCII_NUL
#define ASCII_NUL                       '\0'
#endif


class Buffer {
    private:
        // Volatile members may be accessed from an interrupt service routine
        volatile uint8_t index;
        volatile bool receiving;
        volatile bool sending;
        bool receivingComplete;
        bool sendingComplete;
        uint8_t bufferLength;
        
    public:
        char *buffer;
    
        Buffer(uint8_t size);
        ~Buffer(void);
        void reset(void);
        
        void loadWithOutgoingData(bool *data);
        void loadWithOutgoingData(char *data);
        void loadWithOutgoingData(double *data);
        void loadWithOutgoingData(uint16_t *data);
        void loadWithOutgoingData(uint8_t *data);
        char popByte(void);
        
        void prepareForIncomingData(void);
        void appendByte(char newByte);
        bool isReceiving(void);
        bool isReceivingComplete(void);
        bool isSending(void);
};


#endif
