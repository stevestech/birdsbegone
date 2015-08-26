#include <Arduino.h>

#include "hubMotor.h"
#include "gpio.h"


HubMotor::HubMotor(void) {
    state = STATE_NEUTRAL;
    throttle = 0;
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
    if ((newThrottle >= PWM_MIN) && (newThrottle <= PWM_MAX)) {
        throttle = newThrottle;
    }
    
    else {
        throttle = PWM_MIN;
    }
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