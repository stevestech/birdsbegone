#include <PID_v1.h>
#include <Arduino.h>

#include "actuator.h"
#include "gpio.h"

// Constructor
Actuator::Actuator(void) {
    measuredAngle = analogRead(PIN_A_POSITION_SENSE);

    // There will be no actuator control output until a new
    // desiredAngle is set by an SPI command.
    desiredAngle = measuredAngle;
    controllerOutput = 0;
    
    controller = new PID(&measuredAngle,
                         &controllerOutput,
                         &desiredAngle,
                         GAIN_PROPORTIONAL,
                         GAIN_INTEGRAL,
                         GAIN_DIFFERENTIAL,
                         DIRECT);
                         
    controller.SetMode(AUTOMATIC);
    controller.SetOutputLimits(ACTUATOR_PWM_MAX * -1, ACTUATOR_PWM_MAX);
}


// Destructor
Actuator::~Actuator(void) {
    delete controller;
}


void Actuator::update(void) {
    measuredAngle = analogRead(PIN_A_POSITION_SENSE);
    controller.Compute();
    setActuator(controllerOutput);
}
