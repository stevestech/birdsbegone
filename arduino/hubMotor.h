#ifndef HUBMOTOR_H
#define HUBMOTOR_H

#include <stdint.h>

#define STATE_NEUTRAL                   '0'
#define STATE_BRAKING                   '1'
#define STATE_FORWARD                   '2'
#define STATE_REVERSE                   '3'


class HubMotor {
private:
    char state;
    uint8_t throttle;
    
public:
    void update(void);
}


#endif
