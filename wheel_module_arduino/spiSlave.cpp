#include <Arduino.h>
#include <errno.h>
#include <string.h>

// See header file for a description of the protocol used.
#include "spiSlave.h"
#include "stringToUint.h"

// avr-g++ initialises globals and statics to 0.
// The following globals are accessed from inside the SPI_STC ISR.
volatile uint8_t incomingByte;

// The SPI_STC interrupt service routing will not write to incomingByte while this
// flag is true.
volatile bool incomingByteLocked;

// Echo the last byte that update() has processed, so the master knows where we are at.
volatile uint8_t echoLastByte;


// The following globals are accessed from inside the PCINT0 ISR.
// Used to detect a falling edge on slave select.
volatile bool slaveSelectPrevValue;

// Used to send an error message back to the master.
volatile bool errorCondition;


// Used to hold data when sending or receiving a string.
Buffer stringBuffer(STRING_BUFFER_SIZE);

// When a string is being read, this indicates what the string should do once received.
// It is set by the master before sending the string.
uint8_t purposeForIncomingString;


// SPI has finished reading a byte, and is ready to send one back to the master
ISR (SPI_STC_vect) {
    // Don't write to incomingByte unless the update() method is ready for a new one.
    if (incomingByteLocked) {
        SPDR = NOT_READY;
    }
    
    else {
        incomingByte = SPDR;
        incomingByteLocked = true;
        
        if (errorCondition) {
            SPDR = errorCondition;
        }
        
        else if (stringBuffer.isSending()) {
            // Returns ASCII_NUL if end of buffer reached, or no data loaded into buffer.
            uint8_t nextByte = stringBuffer.popNextByte();
            
            if (nextByte >= FORBIDDEN_RANGE_START) {
                nextByte = MESSAGE_HAS_FORBIDDEN_CHARS;
                errorCondition = MESSAGE_HAS_FORBIDDEN_CHARS;
            }
            
            SPDR = nextByte;
        }
        
        else {
            // Echo the last byte that update() has processed, so the master knows where we are at.
            SPDR = echoLastByte;
        }
    }
}


// On the falling edge of slave select, ensure that the SpiSlave class is ready
// to receive an incoming command from the master.
ISR (PCINT0_vect) {
    bool slaveSelect = digitalRead(SS);
    
    // Detect falling edge
    if (!slaveSelect && slaveSelectPrevValue) {
        // Initialise all flags
        stringBuffer.reset();
        errorCondition = NO_ERROR;
        echoLastByte = ASCII_NUL;
    }
    
    slaveSelectPrevValue = slaveSelect;
}


// Constructor
SpiSlave::SpiSlave(Actuator *actuator, HubMotor *hubMotor) {
    this->actuator = actuator;
    this->hubMotor = hubMotor;
    
    
    /*
     * Setup GPIO
     **/
    pinMode(SS, INPUT);                           // D10
    pinMode(MOSI, INPUT);                         // D11
    pinMode(MISO, OUTPUT);                        // D12
    pinMode(SCLK, INPUT);                         // D13
    
    slaveSelectPrevValue = digitalRead(SS);
    
    // White-list pin PCINT2 (slave select) for pin change interrupts
    bitSet(PCMSK0, PCINT2);
    
    // Enable pin change interrupts for PCINT0 - PCINT7
    bitSet(PCICR, PCIE0);
    
    // Enable the SPI interrupt, which fires when a serial transfer is complete
    bitSet(SPCR, SPIE);
    
    // Enable SPI in slave mode
    bitSet(SPCR, SPE);
}


void SpiSlave::update(void) {
    // SPI interrupt has left data for us to process
    if (incomingByteLocked) {
        if (stringBuffer.isSending()) {
            // Check echo to see if master is receiving our string correctly
            if (incomingByte != stringBuffer.getPrevByte()) {
                errorCondition = MASTER_ECHO_FAILED;
            }
            
            // No further action necessary here, data will be sent
            // automatically when the ISR calls stringBuffer.popNextByte()
        }
        
        else if (stringBuffer.isReceiving()) {
            processIncomingAscii();
        }
        
        else {
            // The first byte received from the master will be a command byte.
            processIncomingCommand();
        }
        
        echoLastByte = incomingByte;
        // Permit the ISR to write to incomingByte again.
        incomingByteLocked = false;
    }
}


void SpiSlave::processIncomingCommand() {
    switch(incomingByte) {
        case RECEIVE_A_ORIENTATION:
            // Update the steering PID controller with a new setpoint from the SPI master
            stringBuffer.prepareForIncomingData();
            purposeForIncomingString = RECEIVE_A_ORIENTATION;
            break;
            
        case RECEIVE_HM_STATE:
            // Update the hub motor state (neutral, braking, forward, reverse)
            stringBuffer.prepareForIncomingData();
            purposeForIncomingString = RECEIVE_HM_STATE;
            break;
            
        case RECEIVE_HM_THROTTLE:
            // Update the hub motor throttle (0 - 255)
            stringBuffer.prepareForIncomingData();
            purposeForIncomingString = RECEIVE_HM_THROTTLE;
            break;
            
        case LOAD_A_MEASURED_ORIENTATION:
            // Load the measured steering orientation from the pot onto the string buffer
            stringBuffer.loadWithOutgoingData(actuator->getMeasuredOrientation());
            break;
            
        case LOAD_A_CONTROLLER_OUTPUT:
            // Load the steering PID control output onto the string buffer
            stringBuffer.loadWithOutgoingData(actuator->getControllerOutput());
            break;
            
        default:
            errorCondition = COMMAND_NOT_RECOGNISED;
            break;
    }
}


void SpiSlave::processIncomingAscii() {
    stringBuffer.appendByte(&incomingByte);
    
    // If this is the last byte in the string, perform any necessary actions
    if (incomingByte == ASCII_NUL) {
        switch(purposeForIncomingString) {
            case RECEIVE_A_ORIENTATION:
                // Update the steering PID controller with a new setpoint from the SPI master
                uint16_t newSetpoint;
                if stringToUint(stringBuffer.buffer, &newSetpoint) {
                    actuator->setDesiredOrientation(&newSetpoint);
                }
                break;
                
            case RECEIVE_HM_STATE:
                // Update the hub motor state (neutral, braking, forward, reverse)
                uint8_t newState;
                if stringToUint(stringBuffer.buffer, &newState) {
                    hubMotor->setState(&newState);
                }
                break;
                
            case RECEIVE_HM_THROTTLE:
                // Update the hub motor throttle (0 - 255)
                uint8_t newThrottle;
                if stringToUint(stringBuffer.buffer, &newThrottle) {
                    hubMotor->setThrottle(&newThrottle);
                }
                break;
        }
    }    
}
