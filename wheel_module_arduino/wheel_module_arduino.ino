#include <PID_v1.h>
#include <SPI.h>

#include "actuator.h"
#include "hubMotor.h"
#include "spiSlave.h"

Actuator actuator;
HubMotor hubMotor;
SpiSlave spiSlave(&actuator, &hubMotor);

    
void setup(void) {
    // Enable if needed for debug
    // Serial.begin(9600);
}


void loop(void) {
    spiSlave.update();
    actuator.update();
    hubMotor.update();
}
