#include <Arduino.h>
#include <stdint.h>

#include "actuator.h"

// Constructor
Actuator::Actuator(bool *emergencyStop) {
    uint8_t const maxControllerOutput = ACTUATOR_PWM_MAX - ACTUATOR_PWM_MIN;

    this->emergencyStop = emergencyStop;
    
    /*
     * Setup PID Controller
     **/
    measuredOrientation = analogRead(PIN_A_POSITION_SENSE);

    // There will be no actuator control output until a new
    // desiredOrientation is set by an SPI command.
    desiredOrientation = measuredOrientation;
    controllerOutput = 0;
    
    controller = new PID(&measuredOrientation,
                         &controllerOutput,
                         &desiredOrientation,
                         GAIN_PROPORTIONAL,
                         GAIN_INTEGRAL,
                         GAIN_DIFFERENTIAL,
                         REVERSE);
                         
    controller->SetMode(AUTOMATIC);
    controller->SetOutputLimits(maxControllerOutput * -1, maxControllerOutput);
    
    /*
     * Setup GPIO
     **/
    // Set pin 9 PWM (Timer 1) switching frequency to 31250 Hz, prescaler = 0b001
    bitSet(TCCR1B, CS00);
    bitClear(TCCR1B, CS01);
    bitClear(TCCR1B, CS02);
    
    // Set pin 3 PWM (Timer 2) switching frequency to 31250 Hz, prescaler = 0b001
    bitSet(TCCR2B, CS00);
    bitClear(TCCR2B, CS01);
    bitClear(TCCR2B, CS02);
    
    // Actuator Pins
    pinMode(PIN_A_THROTTLE_CW, OUTPUT);           // D3 TIMER2B
    pinMode(PIN_A_THROTTLE_ACW, OUTPUT);          // D9 TIMER1A
    pinMode(PIN_A_ENABLE_CW, OUTPUT);
    pinMode(PIN_A_ENABLE_ACW, OUTPUT);
    
    analogWrite(PIN_A_THROTTLE_CW, 0);
    analogWrite(PIN_A_THROTTLE_ACW, 0);
    
    digitalWrite(PIN_A_ENABLE_CW, HIGH);
    digitalWrite(PIN_A_ENABLE_ACW, HIGH);
}


// Destructor
Actuator::~Actuator(void) {
    delete controller;
}    


void Actuator::update() {
    // Get current wheel orientation from potentiometer
    measuredOrientation = analogRead(PIN_A_POSITION_SENSE);
    
    // Protect the potentiometer
    if ((measuredOrientation > WHEEL_ORIENTATION_SHUTDOWN_MAX) ||
        (measuredOrientation < WHEEL_ORIENTATION_SHUTDOWN_MIN)) {
          
        *emergencyStop = true;
    }
    
    if (*emergencyStop) {
        analogWrite(PIN_A_THROTTLE_CW, 0);
        analogWrite(PIN_A_THROTTLE_ACW, 0);
        
        // Only a restart will be able to re-enable these
        digitalWrite(PIN_A_ENABLE_CW, LOW);
        digitalWrite(PIN_A_ENABLE_ACW, LOW);
    }
    
    else {
        // Run the PID controller to get a control signal
        controller->Compute();
        
        // Provide controller output to steering actuator driver
        setMotor();
    }    
}


void Actuator::setMotor(void) {
    // Move in clockwise direction
    if (controllerOutput >= 0) {
        analogWrite(PIN_A_THROTTLE_ACW, 0);
        analogWrite(PIN_A_THROTTLE_CW, (uint8_t)(controllerOutput + ACTUATOR_PWM_MIN));
    }
    
    // Move in anticlockwise direction
    else {
        analogWrite(PIN_A_THROTTLE_CW, 0);
        analogWrite(PIN_A_THROTTLE_ACW, (uint8_t)(controllerOutput * -1 + ACTUATOR_PWM_MIN));
    }
}


double *Actuator::getMeasuredOrientation(void) {
    return &measuredOrientation;
}


void Actuator::setDesiredOrientation(uint16_t *newOrientation) {
    if (*newOrientation > WHEEL_ORIENTATION_LIMIT_MAX) {
        *newOrientation = WHEEL_ORIENTATION_LIMIT_MAX;
    }
    
    else if (*newOrientation < WHEEL_ORIENTATION_LIMIT_MIN) {
        *newOrientation = WHEEL_ORIENTATION_LIMIT_MIN;
    }
    
    desiredOrientation = *newOrientation;
}


double *Actuator::getControllerOutput(void) {
    return &controllerOutput;
}


uint16_t Actuator::readStatusL(void) {
    return analogRead(PIN_A_STATUS_L);
}


uint16_t Actuator::readStatusR(void) {
    return analogRead(PIN_A_STATUS_R);
}




