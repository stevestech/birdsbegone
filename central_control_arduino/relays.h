#ifndef RELAYS_H
#define RELAYS_H

#include <stdint.h>
#include "central_states"

#define DPCO_RELAY_GATE					10     // D10
#define SPNO_RELAY_GATE					11     // D11

// Relay states
#define DPCO_OFF						0
#define DPCO_ON							1
#define SPNO_OFF						0
#define SPNO_ON							1

class Relays {
    private:
	uint8_t dpco_state;
	uint8_t spno_state;
	// PRIVATE VARS INIT HERE
        
    public:
        Relays(void);
        void update(uint8_t *current_state);
};


#endif
