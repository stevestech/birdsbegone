#include <Arduino.h>
#include <SPI.h>
#include <errno.h>
#include <string.h>

// See header file for a description of the protocol used.
#include "spiSlaveCentral.h"
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
SpiSlave::SpiSlave(void) {
    stringBuffer = new Buffer(STRING_BUFFER_SIZE);
	
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


void SpiSlave::update(uint8_t *current_state) {
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
              
            executeIncomingCommand(current_state);
        }
        
        else if ((stringBuffer->isReceiving()) &&
                 (!stringBuffer->isReceivingComplete())) {

            stringBuffer->appendByte(incomingByte);
                
            if (stringBuffer->isReceivingComplete()) {
                // Woot! We got the string, now do something with it.
                executeReceivedString(current_state);
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


void SpiSlave::executeIncomingCommand(uint8_t *current_state) {
    switch(incomingByte) {
		// Instead of three three different receive cmds, could just have a single receive commands
		// which receives a single char to set the state?? Allows to simplify code, but dunno if its
		// worth the implementation time?
		case RECEIVE_SHUTDOWN_CMD:
            *current_state = STATE_SHUTTING_DOWN;
            break;
			
		case RECEIVE_RUNNING_CMD:
            *current_state = STATE_RUNNING;
            break;
			
		case RECEIVE_EMERGENCY_STOP_CMD:
            *current_state = STATE_EMERGENCY_STOP;
            break;
			
        case LOAD_CENTRAL_ARDUINO_STATE:
            stringBuffer->loadWithOutgoingData(*current_state);
            break;
            
        case LOAD_24V_READING:
            stringBuffer->loadWithOutgoingData(battery->getBattery24VReading());
            break;
            
        case LOAD_12V_READING:
            stringBuffer->loadWithOutgoingData(battery->getBattery12VReading());
            break;
		
		case LOAD_ENERGY_CONSUMED:
            stringBuffer->loadWithOutgoingData(battery->getEnergyConsumed());
            break;
			
        default:
            errorCondition = COMMAND_NOT_RECOGNISED;
            break;
    }
}


void SpiSlave::executeReceivedString(uint8_t *current_state) {
    switch(purposeForIncomingString) {
		// INPUT SWITCH CASES FOR RECIEVING STRING
    }    
}


uint8_t *SpiSlave::getShutdownError(void) {
    uint8_t error;
    
    if (actuator->unsafeOrientation) {
        error = UNSAFE_ORIENTATION_SHUTDOWN;
    }
    
    else {
        error = ALL_OK;
    }
    
    return &error;
}

