#include <Arduino.h>
#include <errno.h>
#include <string.h>

// See header file for a description of the protocol used.
#include "spiSlave.h"

// Constructor
SpiSlave::SpiSlave(Actuator *actuator, HubMotor *hubMotor) {
    // Used to hold data when sending or receiving a string
    stringBuffer = new char[STRING_BUFFER_SIZE];
    
    // When a string is being read, this indicates what the string should do once received.
    // It is set by the master before sending the string. Unless the master has indicated that
    // it will be sending ASCII, this will be set to NOT_RECEIVING_ASCII.
    purposeForIncomingString = NOT_RECEIVING_ASCII;
    
    this->actuator = actuator;
    this->hubMotor = hubMotor;
    
    
    /*
     * Setup GPIO
     **/
    pinMode(SS, INPUT);                           // D10
    pinMode(MOSI, INPUT);                         // D11
    pinMode(MISO, OUTPUT);                        // D12
    pinMode(SCLK, INPUT);                         // D13
    
    // White-list pin PCINT2 (slave select) for pin change interrupts
    bitSet(PCMSK0, PCINT2);
    
    // Enable pin change interrupts for PCINT0 - PCINT7
    bitSet(PCICR, PCIE0);
    
    // Enable the SPI interrupt, which fires when a serial transfer is complete
    bitSet(SPCR, SPIE);
    
    // Enable SPI in slave mode
    bitSet(SPCR, SPE);
}


// Destructor
SpiSlave::~SpiSlave(void) {
    delete buffer;
}


void SpiSlave::retrieveIncomingByte(void) {
    uint8_t incomingByte = SPDR;
    
    // SPI master is in the process of sending an ASCII string
    if (purposeForIncomingString) {
        processIncomingAscii(incomingByte);
    }
    
    else {
        processIncomingCommand(incomingByte);
    }
}


void SpiSlave::resetBuffer(void) {
    stringBufferIndex = 0;
}


void SpiSlave::sendCharFromBuffer(void) {
    // If we have reached the end of the buffer, send CHAR_ETX
    if (stringBufferIndex >= sizeof(stringBuffer)) {
        SPDR = ASCII_ETX;
    }
    
    else {
        SPDR = stringBuffer[stringBufferIndex++];
    }
}


void SpiSlave::processIncomingAscii(char incomingByte) {
    // This is the last byte in the string, so perform any necessary actions
    if (incomingByte == ASCII_ETX) {
        // Instead of appending ETX to the string, terminate it with a NUL character.
        incomingByte = '\0';
        
        switch(purposeForIncomingString) {
        case RECEIVE_A_ORIENTATION:
            actuator->setDesiredOrientation((uint16_t) strtol()
        }
    }    
    
    // Append incomingByte to the stringBuffer if there is room
    if (stringBufferIndex < sizeof(stringBuffer)) {
        stringBuffer[stringBufferIndex++] = incomingByte;
    }
}


void SpiSlave::processIncomingCommand(uint8_t command) {
    switch(command) {
    case 
    }
}


bool SpiSlave::stringToUint16(const char *string, uint16_t *result) {
    // Set to the next character in string after the numerical value, hopefully this is the null terminator
    char *end;
    
    // Used to detect overflow errors while running strtol
    errno = 0;
    
    // First, convert the string to a long
    long longResult = strtol(string, &end, BASE_TEN);
    
    // Only proceed if the string is a valid uint16_t
    if (errno ||                        // Set in the event of an overflow condition
        end == string ||                // No conversion was performed
        *end != '\0' ||                 // The entire string was not converted
        longResult < 0 ||               // The value is not in the valid range for a uint16_t
        longResult >= UINT16_MAX) {
            
            return false;
    }
    
    *result = (uint16_t) longResult;
    return true;
}
