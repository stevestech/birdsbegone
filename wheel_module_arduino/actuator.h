#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <PID_v1.h>

// Actuator pins
#define PIN_A_THROTTLE_CW               3      // D3 Timer2B
#define PIN_A_THROTTLE_ACW              9      // D9 Timer1A
#define PIN_A_POSITION_SENSE            0      // A0
#define PIN_A_STATUS_L                  1      // A1
#define PIN_A_STATUS_R                  2      // A2

/*
 * PWM Stuff
 * A PWM value of 255 corresponds to 100% duty cycle.
 * 
 * ACTUATOR_PWM_MIN is the minimum duty cycle that causes the actuator to begin rotating. As a
 * high PWM switching frequency is used, slew rate limitations produce a deadband at low PWMs.
 * This offset is added to the control output to improve the linearity of the response.
 * Some deadband is beneficial in preventing the actuator from responding to low-magnitude
 * noise on the analogue potentiometer input.
 *
 * A high PWM switching frequency is required so that the actuator voltage waveform is overdamped.
 * At lower frequencies the system is underdamped and can ring, producing unwanted acoustic effects
 * from the actuator.
 * 
 * Higher value, less deadband.
 * Observed valid range @ fs=31.25kHz: 0 to 72
 * Recommended @ fs=31.25kHz: 60
 *
 *
 * ACTUATOR_PWM_MAX limits the max duty cycle that can be applied to the actuator driver.
 * The PID controller will output between 0 and (ACTUATOR_PWM_MAX - ACTUATOR_PWM_MIN).
 * This maps to a PWM output between ACTUATOR_PWM_MIN and ACTUATOR_PWM_MAX.
 * 
 * Valid range: (ACTUATOR_PWM_MIN + 1) to (PWM_MAX - ACTUATOR_PWM_OFFSET)
 **/
#define ACTUATOR_PWM_MIN                60
#define ACTUATOR_PWM_MAX                127

// Actuator PID gains
#define GAIN_PROPORTIONAL               1
#define GAIN_INTEGRAL                   0
#define GAIN_DIFFERENTIAL               0

class Actuator {
    private:
        double desiredOrientation;
        double measuredOrientation;
        double controllerOutput;
            
        PID* controller;
        
        void setMotor(void);
        
    public:
        Actuator(void);
        ~Actuator(void);
        void update(void);
        double *getMeasuredOrientation(void);
        double *getControllerOutput(void);
        void setDesiredOrientation(uint16_t *newOrientation);
};

#endif
