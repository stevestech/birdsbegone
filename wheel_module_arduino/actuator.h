#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <PID_v1.h>

// Actuator pins
#define PIN_A_THROTTLE_CW               3      // D3 Timer2B
#define PIN_A_THROTTLE_ACW              9      // D9 Timer1A
#define PIN_A_ENABLE_CW                 0      // D0
#define PIN_A_ENABLE_ACW                1      // D1

#define PIN_A_POSITION_SENSE            7      // A7
#define PIN_A_STATUS_L                  6      // A6
#define PIN_A_STATUS_R                  5      // A5

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
 * Recommended @ fs=31.25kHz: 55
 *
 *
 * ACTUATOR_PWM_MAX limits the max duty cycle that can be applied to the actuator driver.
 * The PID controller will output between 0 and (ACTUATOR_PWM_MAX - ACTUATOR_PWM_MIN).
 * This maps to a PWM output between ACTUATOR_PWM_MIN and ACTUATOR_PWM_MAX.
 * 
 * Valid range: (ACTUATOR_PWM_MIN + 1) to (PWM_MAX - ACTUATOR_PWM_MIN)
 **/
#define ACTUATOR_PWM_MIN                    55
#define ACTUATOR_PWM_MAX                    127

// Won't accept setpoints from the master outside of these limits
#define WHEEL_ORIENTATION_LIMIT_MIN         350
#define WHEEL_ORIENTATION_LIMIT_MAX         650

// Will shutdown actuator motor if wheel orientation found outside these limits
#define WHEEL_ORIENTATION_SHUTDOWN_MIN      250
#define WHEEL_ORIENTATION_SHUTDOWN_MAX      750

// Actuator PID gains
#define GAIN_PROPORTIONAL                   1
#define GAIN_INTEGRAL                       0.2
#define GAIN_DIFFERENTIAL                   0

class Actuator {
    private:
        double desiredOrientation;
        double measuredOrientation;
        double controllerOutput;
        PID* controller;
        bool *emergencyStop;
        
        void setMotor(void);
        
    public:
        bool gotFirstSpiPacket;
    
        Actuator(bool *emergencyStop);
        ~Actuator(void);
        void init(void);
        void update(void);
        double *getMeasuredOrientation(void);
        double *getControllerOutput(void);
        void setDesiredOrientation(uint16_t *newOrientation);
        uint16_t readStatusL(void);
        uint16_t readStatusR(void);
};

#endif
