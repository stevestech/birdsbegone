#include <Arduino.h>

#include "hubMotor.h"


volatile uint16_t wheelSpeedCurrent;
volatile uint16_t wheelSpeedPrevious;


HubMotor::HubMotor(bool *emergencyStop) {
    state = STATE_NEUTRAL;
    throttle = 0;
    this->emergencyStop = emergencyStop;
}

void HubMotor::init(void) {
    /*
     * Setup GPIO
     **/
    pinMode(PIN_HM_SPEED_SENSE, INPUT);
    pinMode(PIN_HM_BRAKE, OUTPUT);
    pinMode(PIN_HM_THROTTLE, OUTPUT);
    pinMode(PIN_HM_REVERSE, OUTPUT);
    
    analogWrite(PIN_HM_THROTTLE, 0);
    
    digitalWrite(PIN_HM_BRAKE, HIGH);
    digitalWrite(PIN_HM_REVERSE, HIGH);
    
    // Count the rising edges of the hall effect sensor to determine wheel rpm
    attachInterrupt(digitalPinToInterrupt(2), incrementSpeedSensorCount, RISING);
    
    // Enable overflow interrupts for timer 1
    bitSet(TIMSK1, TOIE1);
}  


void HubMotor::update(void) { 
    if (*emergencyStop) {
        state = STATE_NEUTRAL;
        throttle = 0;
    }
  
    switch(state) {
    default:
    case STATE_NEUTRAL:
        analogWrite(PIN_HM_THROTTLE, 0);
        digitalWrite(PIN_HM_BRAKE, HIGH);
        digitalWrite(PIN_HM_REVERSE, HIGH);
        break;

    case STATE_BRAKING:
        analogWrite(PIN_HM_THROTTLE, 0);
        digitalWrite(PIN_HM_BRAKE, LOW);
        digitalWrite(PIN_HM_REVERSE, HIGH);
        break;

    case STATE_FORWARD:
        analogWrite(PIN_HM_THROTTLE, throttle);
        digitalWrite(PIN_HM_BRAKE, HIGH);
        digitalWrite(PIN_HM_REVERSE, HIGH);
        break;

    case STATE_REVERSE:
        analogWrite(PIN_HM_THROTTLE, throttle);
        digitalWrite(PIN_HM_BRAKE, HIGH);
        digitalWrite(PIN_HM_REVERSE, LOW);
        break;
    }    
}


void HubMotor::setThrottle(uint8_t *newThrottle) {
    throttle = *newThrottle;
}


void HubMotor::setState(uint8_t *newState) {
    if ((*newState == STATE_NEUTRAL) ||
        (*newState == STATE_BRAKING) ||
        (*newState == STATE_FORWARD) ||
        (*newState == STATE_REVERSE)) {
            
        state = *newState;
    }
    
    else {
        state = STATE_NEUTRAL;
    }
}


void incrementSpeedSensorCount(void) {
    wheelSpeedCurrent++;
}


// Timer 1 overflow handler, use to determine wheel speed
ISR(TIMER1_OVF_vect) {
    wheelSpeedPrevious = wheelSpeedCurrent;
    wheelSpeedCurrent = 0;
}


uint16_t HubMotor::getWheelSpeed(void) {
    return wheelSpeedPrevious;
}

