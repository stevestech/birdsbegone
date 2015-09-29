#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

// Hub motor pins
#define STOP_LED              			5      // D5
#define STOP_READ                       6      // D6
#define START_LED						7      // D7

// MOVE THESE INTO ANOTHER MODULE
#define DPCO_RELAY_GATE					10     // D10
#define SPNO_RELAY_GATE					11     // D11

// MOVE THESE INTO ANOTHER MODULE
#define CURRENT_SENSE					19 	   // A0
#define BATTERY_READ_1					21	   // A2
#define BATTERY_READ_2					20	   // A1


class Buttons {
    private:
		bool SPI_recieved;
		bool shutting_down;
        
    public:
        Buttons(void);
        void update(void);
        //void setThrottle(uint8_t *newThrottle);
        //void setState(uint8_t *newState);
};


#endif
