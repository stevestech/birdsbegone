#include <PID_v1.h>
#include <SPI.h>

#include "actuator.h"
#include "hubMotor.h"
#include "spiSlave.h"
#include "timers.h"

bool emergencyStop = false;

Timers timers(&emergencyStop);
Actuator actuator(&emergencyStop);
HubMotor hubMotor(&emergencyStop);
SpiSlave spiSlave(&emergencyStop, &actuator, &hubMotor, &timers);

    
void setup(void) {
}


void loop(void) {
    spiSlave.update();
    actuator.update();
    hubMotor.update();
    timers.update();
}
