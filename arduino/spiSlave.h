#ifndef SPISLAVE_H
#define SPISLAVE_H

#include <stdint.h>

#define SPI_BUFFER_SIZE                 16
#define CHAR_ETX                        3

/*
 * SPI Operations
 **/
 #define SET_A_ORIENTATION              16
 #define SET_HM_STATE                   17
 #define SET_HM_THROTTLE                18
 
 #define GET_A_MEASURED_ORIENTATION     19
 #define GET_A_CONTROLLER_OUTPUT        20


class SpiSlave {
private:
    volatile uint8_t operationType;
    volatile uint8_t *operationBuffer;
    volatile uint8_t operationBufferIndex;
    
    void getReadyForNextOperation(void);
    void retrieveIncomingByte(void);
    
public:
    SpiSlave(void);
    ~SpiSlave(void);
}
    

#endif