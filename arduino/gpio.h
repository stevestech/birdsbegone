#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// Hub motor pins
#define PIN_HM_THROTTLE                 5      // D5 Timer0B
#define PIN_HM_BRAKE                    4      // D4
#define PIN_HM_REVERSE                  7      // D7

// Actuator pins
#define PIN_A_THROTTLE_CW               3      // D3 Timer2B
#define PIN_A_THROTTLE_ACW              9      // D9 Timer1A
#define PIN_A_POSITION_SENSE            0      // A0
#define PIN_A_STATUS_L                  1      // A1
#define PIN_A_STATUS_R                  2      // A2

// PWM Stuff
// 255 corresponds to 100% duty cycle.
// This is used to counteract the deadzone caused by slew rate from high frequency PWM. High value, less dead zone. Max 72.
#define ACTUATOR_PWM_MIN                60
// Limit the max duty cycle that can be applied to the actuator driver.
#define ACTUATOR_PWM_MAX                (127 - ACTUATOR_PWM_MIN)

#define TCCRxB_PRESCALER_MASK           0b11111000

void initialiseGpio(void);
void setTimerPrescaler(uint8_t timer, uint8_t prescaler);
void setActuator(double controllerOutput);

#endif