#include <PID_v1.h>
#include <SPI.h>

#include "actuator.h"
#include "hubMotor.h"
#include "spiSlave.h"

bool emergencyStop = false;

Actuator actuator(&emergencyStop);
HubMotor hubMotor(&emergencyStop);
SpiSlave spiSlave(&emergencyStop, &actuator, &hubMotor);

    
void setup(void) {
    // Enable if needed for debug
    // Serial.begin(9600);
}


void loop(void) {
    spiSlave.update();
    actuator.update();
    hubMotor.update();
}
