#ifndef HUBMOTOR_H
#define HUBMOTOR_H

#include <stdint.h>

#define STATE_NEUTRAL                   0
#define STATE_BRAKING                   1
#define STATE_FORWARD                   2
#define STATE_REVERSE                   3

// Hub motor pins
#define PIN_HM_SPEED_SENSE              2      // D2
#define PIN_HM_BRAKE                    4      // D4
#define PIN_HM_THROTTLE                 5      // D5 Timer0B
#define PIN_HM_REVERSE                  7      // D7

#define PWM_MAX                         255
#define PWM_MIN                         0


class HubMotor {
    private:
        uint8_t state;
        uint8_t throttle;
        
    public:
        HubMotor(void);
        void update(void);
        void setThrottle(uint8_t *newThrottle);
        void setState(uint8_t *newState);
};


#endif
