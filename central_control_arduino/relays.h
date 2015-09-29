#ifndef RELAYS_H
#define RELAYS_H

#include <stdint.h>

#define DPCO_RELAY_GATE					10     // D10
#define SPNO_RELAY_GATE					11     // D11

class Relays {
    private:
		// PRIVATE VARS INIT HERE
        
    public:
        Buttons(void);
        void update(void);
        //void setThrottle(uint8_t *newThrottle);
        //void setState(uint8_t *newState);
};


#endif
