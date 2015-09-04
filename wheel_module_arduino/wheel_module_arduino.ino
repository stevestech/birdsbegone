#include <PID_v1.h>
#include <SPI.h>

#include "actuator.h"
#include "hubMotor.h"
#include "spiSlave.h"

Actuator *actuator;
HubMotor *hubMotor;
SpiSlave *spiSlave;

    
void setup(void)
{
    actuator = new Actuator();
    hubMotor = new HubMotor();
    spiSlave = new SpiSlave(actuator, hubMotor);
}


void loop(void)
{
    spiSlave->update();
    actuator->update();
    hubMotor->update();
}
