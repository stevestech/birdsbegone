#include <PID_v1.h>
#include <SPI.h>

#include "spiSlave.h"

SpiSlave spiSlave(&actuator, &hubMotor);

void setup(void) {
    Serial.begin(9600);
}

void loop(void) {
    spiSlave.update();
}
