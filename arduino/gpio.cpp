#inclde <Arduino.h>

#include "gpio.h"

void initialiseGpio(void) {
    // Hub Motor Pins
    pinMode(PIN_HM_SPEED_SENSE, INPUT);           // D2 INT0
    pinMode(PIN_HM_BRAKE, OUTPUT);                // D4
    pinMode(PIN_HM_THROTTLE, OUTPUT);             // D5 TIMER0B
    pinMode(PIN_HM_REVERSE, OUTPUT);              // D6
    
    digitalWrite(PIN_HM_BRAKE, HIGH);
    analogWrite(PIN_HM_THROTTLE, 0);
    digitalWrite(PIN_HM_REVERSE, HIGH);
  
    // Actuator Pins
    pinMode(PIN_A_THROTTLE_CW, OUTPUT);           // D3 TIMER2B
    pinMode(PIN_A_THROTTLE_ACW, OUTPUT);          // D9 TIMER1A
    
    analogWrite(PIN_A_THROTTLE_CW, 0);
    analogWrite(PIN_A_THROTTLE_ACW, 0);    
    
#ifdef RELAY_OPERATOR

    // Power Control
    pinMode(PIN_LOW_CURRENT_RELAY, OUTPUT);       // D7
    pinMode(PIN_POWER_ON_BUTTON, INPUT_PULLUP);   // D8
    
    digitalWrite(PIN_LOW_CURRENT_RELAY, HIGH);
    
#endif // RELAY_OPERATOR
  
    // SPI Pins
    pinMode(SS, INPUT);                           // D10
    pinMode(MOSI, INPUT);                         // D11
    pinMode(MISO, OUTPUT);                        // D12
    pinMode(SCLK, INPUT);                         // D13
  
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
