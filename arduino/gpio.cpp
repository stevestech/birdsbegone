#inclde <Arduino.h>

#include "gpio.h"

void initialiseGpio(void) {
  // Have to send on MISO
  pinMode(MISO, OUTPUT);
  pinMode(PIN_HM_THROTTLE, OUTPUT);
  pinMode(PIN_HM_BRAKE, OUTPUT);
  pinMode(PIN_HM_REVERSE, OUTPUT);
  
  pinMode(PIN_A_THROTTLE_CW, OUTPUT);
  pinMode(PIN_A_THROTTLE_ACW, OUTPUT);
  
  analogWrite(PIN_HM_THROTTLE, 0);
  digitalWrite(PIN_HM_BRAKE, HIGH);
  digitalWrite(PIN_HM_REVERSE, HIGH);
  
  analogWrite(PIN_A_THROTTLE_CW, 0);
  analogWrite(PIN_A_THROTTLE_ACW, 0);  
  
  // Set pin 3 PWM (Timer 2) switching frequency to 31250 Hz
  setTimerPrescaler(2, 1);
  // Set pin 9 PWM (Timer 1) switching frequency to 31250 Hz
  setTimerPrescaler(1, 1);
}


void setTimerPrescaler(uint8_t timer, uint8_t prescaler) {
    volatile uint8_t *TCCRxB;
    
    switch(timer) {
    case 0:
        TCCRxB = &TCCR0B;
        break;
        
    case 1:
        TCCRxB = &TCCR1B;
        break;
        
    case 2:
        TCCRxB = &TCCR2B;
        break;
        
    default:
        return;
    }
    
    // Turn off the prescaler bits
    *TCCRxB &= TCCRxB_PRESCALER_MASK;
    // Turn off everything apart from the prescaler bits
    prescaler &= ~TCCRxB_PRESCALER_MASK;
    
    *TCCRxB |= prescaler;
}


void setActuator(double controllerOutput) {
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
