#include <Arduino.h>

// Constructor
SpiSlave::SpiSlave(void) {
    operationBuffer = new uint8_t[SPI_BUFFER_SIZE];
    
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
    delete operationBuffer;
}


