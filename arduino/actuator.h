#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <PID_v1.h>

// Actuator PID gains
#define GAIN_PROPORTIONAL               1
#define GAIN_INTEGRAL                   0
#define GAIN_DIFFERENTIAL               0

class Actuator {
private:
    double desiredAngle;
    double measuredAngle;
    double controllerOutput;
        
    PID* controller;
    
public:
    Actuator(void);
    ~Actuator(void);
    void update(void);
    double getOutput(void);
};

#endif