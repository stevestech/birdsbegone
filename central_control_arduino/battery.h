#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>
#include "central_states"

// Battery related pin map
#define CURRENT_SENSE					19 	   // A0
#define BATTERY_READ_1					21	   // A2
#define BATTERY_READ_2					20	   // A1

class Battery {
    private:
		double battery_24v_reading;
		double battery_12v_reading;
		double energy_consumed;
    public:
        Battery(void);
        void update(uint8_t *current_state);
		double *getBattery24VReading(void);
		double *getBattery12VReading(void);
		double *getEnergyConsumed(void);
};


#endif