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

/*
 * PWM Stuff
 * A PWM value of 255 corresponds to 100% duty cycle.
 * ACTUATOR_PWM_MIN is the minimum duty cycle that causes the actuator to begin rotating. As a
 * high PWM switching frequency is used, slew rate limitations produce a deadband.
 * This offset is added to the control output, so that a linear motor response is achieved.
 * Some deadband is beneficial in preventing the actuator from responding to low-magnitude
 * noise on the analogue potentiometer input.
 *
 * A high PWM switching frequency is required so that the actuator voltage waveform is overdamped.
 * At lower frequencies the system is underdamped and can ring, producing unwanted acoustic effects
 * from the actuator.
 * 
 * Higher value, less deadband. Max 72.
 **/
#define ACTUATOR_PWM_MIN                60
// Limit the max duty cycle that can be applied to the actuator driver.
#define ACTUATOR_PWM_MAX                (127 - ACTUATOR_PWM_MIN)
#define PWM_MAX                         255
#define PWM_MIN                         0

#define TCCRxB_PRESCALER_MASK           0b11111000

void initialiseGpio(void);
void setTimerPrescaler(uint8_t timer, uint8_t prescaler);
void setActuator(double controllerOutput);

#endif