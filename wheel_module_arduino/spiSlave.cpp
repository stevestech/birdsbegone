#include <Arduino.h>
#include <SPI.h>
#include <errno.h>
#include <string.h>

// See header file for a description of the protocol used.
#include "spiSlave.h"
#include "stringToUint.h"



// SPI has finished reading a byte, and is ready to send one back to the master
ISR (SPI_STC_vect) {
    // Don't write to incomingByte unless the update() method is ready for a new one.
    if (SpiSlave::incomingByteLocked) {
        SPDR = NOT_READY;
    }
    
    else {
        SpiSlave::incomingByte = SPDR;
        SPDR = SpiSlave::outgoingByte;
        
        SpiSlave::incomingByteLocked = true;
    }
}


// On the falling edge of slave select, ensure that the SpiSlave class is ready
// to receive an incoming command from the master.
ISR (PCINT0_vect) {
    bool slaveSelect = digitalRead(SS);
    
    // Detect falling edge
    if (!slaveSelect && SpiSlave::slaveSelectPrevValue) {
        SpiSlave::slaveSelectFallingEdge = true;        
    }
    
    SpiSlave::slaveSelectPrevValue = slaveSelect;
}


// Constructor
SpiSlave::SpiSlave(Actuator *actuator, HubMotor *hubMotor) {
    stringBuffer = new StringBuffer(STRING_BUFFER_SIZE);
    
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
    
    
    // Initialise flags and variables
    reset();
}


SpiSlave::~SpiSlave(void) {
    delete stringBuffer;
}


// Called at the start of a new operation, after SS falling edge.
// Reset all flags and state variables relating to the previous operation.
void SpiSlave::reset(void) {
    stringBuffer->reset();
    errorCondition = NO_ERROR;
    purposeForIncomingString = NO_INCOMING_STRING;
}


void SpiSlave::update(void) {
    if (slaveSelectFallingEdge) {
        slaveSelectFallingEdge = false;
        reset();
    }
    
    // SPI interrupt has left data for us to process
    if (incomingByteLocked) {
        if (stringBuffer->isSending()) {
            #ifdef ENABLE_ECHO_VERIFICATION
                // Check echo to see if master is receiving our string correctly
                if (incomingByte != stringBuffer->getPrevByte()) {
                    errorCondition = MASTER_ECHO_FAILED;
                }
            #endif
            

            outgoingByte = stringBuffer->popNextByte();
            
            if (outgoingByte >= FORBIDDEN_RANGE_START) {
                errorCondition = MESSAGE_HAS_FORBIDDEN_CHARS;
            }
        }
        
        else if (stringBuffer->isReceiving()) {
            #ifdef ENABLE_ECHO_VERIFICATION
                // Echo the input so that the master to verify that we got the correct message.
                outgoingByte = incomingByte;
            #else
                outgoingByte = EMPTY_BYTE;
            #endif
            
            if (!stringBuffer->isReceivingComplete()) {
                stringBuffer->appendByte(&incomingByte);
                
                if (stringBuffer->isReceivingComplete()) {
                    // Woot! We got the string, now do something with it.
                    executeReceivedString();
                }
            }
        }
        
        // The first byte received from the master after SS falling edge will be a command byte.
        else {
            #ifdef ENABLE_ECHO_VERIFICATION
                outgoingByte = incomingByte;
            #else
                outgoingByte = EMPTY_BYTE;
            #endif
            
            executeIncomingCommand();
        }
        
        if (errorCondition) {
            outgoingByte = errorCondition;
        }        
        
        // Permit the ISR to write to incomingByte again.
        incomingByteLocked = false;
    }
}


void SpiSlave::executeIncomingCommand() {
    switch(incomingByte) {
        case RECEIVE_A_ORIENTATION:
            // Update the steering PID controller with a new setpoint from the SPI master
            stringBuffer->prepareForIncomingData();
            purposeForIncomingString = RECEIVE_A_ORIENTATION;
            break;
            
        case RECEIVE_HM_STATE:
            // Update the hub motor state (neutral, braking, forward, reverse)
            stringBuffer->prepareForIncomingData();
            purposeForIncomingString = RECEIVE_HM_STATE;
            break;
            
        case RECEIVE_HM_THROTTLE:
            // Update the hub motor throttle (0 - 255)
            stringBuffer->prepareForIncomingData();
            purposeForIncomingString = RECEIVE_HM_THROTTLE;
            break;
            
        case LOAD_A_MEASURED_ORIENTATION:
            // Load the measured steering orientation from the pot onto the string buffer
            stringBuffer->loadWithOutgoingData(actuator->getMeasuredOrientation());
            break;
            
        case LOAD_A_CONTROLLER_OUTPUT:
            // Load the steering PID control output onto the string buffer
            stringBuffer->loadWithOutgoingData(actuator->getControllerOutput());
            break;
            
        default:
            errorCondition = COMMAND_NOT_RECOGNISED;
            break;
    }
}


void SpiSlave::executeReceivedString() {
    switch(purposeForIncomingString) {
        case RECEIVE_A_ORIENTATION:
            // Update the steering PID controller with a new setpoint from the SPI master
            uint16_t newSetpoint;
            if stringToUint(stringBuffer->buffer, &newSetpoint) {
                actuator->setDesiredOrientation(&newSetpoint);
            }
            break;
            
        case RECEIVE_HM_STATE:
            // Update the hub motor state (neutral, braking, forward, reverse)
            uint8_t newState;
            if stringToUint(stringBuffer->buffer, &newState) {
                hubMotor->setState(&newState);
            }
            break;
            
        case RECEIVE_HM_THROTTLE:
            // Update the hub motor throttle (0 - 255)
            uint8_t newThrottle;
            if stringToUint(stringBuffer->buffer, &newThrottle) {
                hubMotor->setThrottle(&newThrottle);
            }
            break;
    }    
}
