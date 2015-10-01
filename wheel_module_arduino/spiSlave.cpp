#include <Arduino.h>
#include <SPI.h>
#include <errno.h>
#include <string.h>

// See header file for a description of the protocol used.
#include "spiSlave.h"
#include "stringToUint.h"


// avr-g++ initialises globals and statics to 0.
// Volatile globals are accessed by interrupt service routines.
volatile uint8_t incomingByte;

// The SPI_STC interrupt service routine will not write to incomingByte while this
// flag is true.
volatile bool incomingByteReady;

// Used to detect a falling edge on slave select.
volatile bool slaveSelectPrevValue;

// Flag set true after a falling edge has been detected.
volatile bool slaveSelectFallingEdge;



// SPI has finished reading a byte, and is ready to send one back to the master
ISR (SPI_STC_vect) {
    // Don't write to incomingByte unless the update() method is ready for a new one.
    if (!incomingByteReady) {
        incomingByte = SPDR;
        incomingByteReady = true;
        // Once update() has real data to output, SPDR will be updated.
        SPDR = NOT_READY;
    }
}


// On the falling edge of slave select, ensure that the SpiSlave class is ready
// to receive an incoming command from the master.
ISR (PCINT0_vect) {
    bool slaveSelect = digitalRead(SS);
    
    // Detect falling edge
    if (!slaveSelect && slaveSelectPrevValue) {
        slaveSelectFallingEdge = true;
    }
    
    slaveSelectPrevValue = slaveSelect;
}


// Constructor
SpiSlave::SpiSlave(bool *emergencyStop, Actuator *actuator, HubMotor *hubMotor) {
    stringBuffer = new Buffer(STRING_BUFFER_SIZE);
    
    this->actuator = actuator;
    this->hubMotor = hubMotor;
    this->emergencyStop = emergencyStop;
    
    /*
     * Setup GPIO
     **/
    pinMode(SS, INPUT);                           // D10
    pinMode(MOSI, INPUT);                         // D11
    pinMode(MISO, OUTPUT);                        // D12
    pinMode(SCK, INPUT);                          // D13
    
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
    SPDR = EMPTY_BYTE;
    incomingByteReady = false;
    firstByte = true;
}


void SpiSlave::update(void) {
    if (slaveSelectFallingEdge) {
        slaveSelectFallingEdge = false;
        reset();
    }
    
    // SPI interrupt has left data for us to process
    if (incomingByteReady) {
        uint8_t outgoingByte = EMPTY_BYTE;
        
        // The first byte received from the master after SS falling edge will be a command byte.
        if ((!stringBuffer->isSending()) &&
            (!stringBuffer->isReceiving())) {
              
            executeIncomingCommand();
        }
        
        else if ((stringBuffer->isReceiving()) &&
                 (!stringBuffer->isReceivingComplete())) {

            stringBuffer->appendByte(incomingByte);
                
            if (stringBuffer->isReceivingComplete()) {
                // Woot! We got the string, now do something with it.
                executeReceivedString();
            }
        }
        
        // If we just executed a command to load the string buffer with data,
        // send the first byte straight away! (Hence the missing "else")
        if (stringBuffer->isSending()) {
            outgoingByte = stringBuffer->popByte();
            
            if (outgoingByte >= FORBIDDEN_RANGE_START) {
                errorCondition = MESSAGE_HAS_FORBIDDEN_CHARS;
            }
        }
        
        if (errorCondition) {
            outgoingByte = errorCondition;
        }
        
        // Permit the ISR to write to incomingByte again.
        SPDR = outgoingByte;
        incomingByteReady = false;
        firstByte = false;
    }
}


void SpiSlave::executeIncomingCommand() {
    uint16_t data;
  
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
            
        case LOAD_EMERGENCY_STOP:
            stringBuffer->loadWithOutgoingData(emergencyStop);
            break;
            
        case LOAD_ACTUATOR_STATUS_L:
            data = actuator->readStatusL();
            stringBuffer->loadWithOutgoingData(&data);
            break;
            
        case LOAD_ACTUATOR_STATUS_R:
            data = actuator->readStatusR();        
            stringBuffer->loadWithOutgoingData(&data);
            break;
            
        case LOAD_HUB_MOTOR_SPEED:
            data = hubMotor->getWheelSpeed();
            stringBuffer->loadWithOutgoingData(&data);
        
        case SET_EMERGENCY_STOP:
            *emergencyStop = true;
            break;
            
        default:
            errorCondition = COMMAND_NOT_RECOGNISED;
            break;
    }
}


void SpiSlave::executeReceivedString(void) {
    switch(purposeForIncomingString) {
        case RECEIVE_A_ORIENTATION:
            // Update the steering PID controller with a new setpoint from the SPI master
            uint16_t newSetpoint;
            if (stringToUint(stringBuffer->buffer, &newSetpoint)) {
                actuator->setDesiredOrientation(&newSetpoint);
            }
            break;
            
        case RECEIVE_HM_STATE:
            // Update the hub motor state (neutral, braking, forward, reverse)
            uint8_t newState;
            if (stringToUint(stringBuffer->buffer, &newState)) {
                hubMotor->setState(&newState);
            }
            break;
            
        case RECEIVE_HM_THROTTLE:
            // Update the hub motor throttle (0 - 255)
            uint8_t newThrottle;
            if (stringToUint(stringBuffer->buffer, &newThrottle)) {
                hubMotor->setThrottle(&newThrottle);
            }
            break;
    }    
}

