#include <Arduino.h>

#include "hubMotor.h"


HubMotor::HubMotor(void) {
    state = STATE_NEUTRAL;
    throttle = 0;
    
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
}


void HubMotor::update(void) {
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
