#include <Arduino.h>

#include "hubMotor.h"


HubMotor::HubMotor(void) {
    state = STATE_NEUTRAL;
    throttle = 0;
    
    /*
     * Setup GPIO
     **/
    pinMode(PIN_HM_SPEED_SENSE, INPUT);           // D2 INT0
    pinMode(PIN_HM_BRAKE, OUTPUT);                // D4
    pinMode(PIN_HM_THROTTLE, OUTPUT);             // D5 TIMER0B
    pinMode(PIN_HM_REVERSE, OUTPUT);              // D6
    
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


void HubMotor::setThrottle(uint8_t newThrottle) {
    throttle = newThrottle;
}


void HubMotor::setState(char newState) {
    switch(newState) {
    case STATE_NEUTRAL:
    case STATE_BRAKING:
    case STATE_FORWARD:
    case STATE_REVERSE:
        state = newState;
        break;
        
    default:
        state = STATE_NEUTRAL;
        break;
    }
}