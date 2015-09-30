#include <Arduino.h>

#include "relays.h"

Relays::Relays(void) {
	
	dpco_state = DPCO_ON;
	spno_state = SPNO_OFF;
    /*
     * Setup GPIO
     **/
	pinMode(DPCO_RELAY_GATE, OUTPUT);
    pinMode(SPNO_RELAY_GATE, OUTPUT);
    
	// Toggle on DPCO relay on start-up, latching power on.
    digitalWrite(DPCO_RELAY_GATE, dpco_state);
	// Keep hub motors off until fully turned on
    digitalWrite(SPNO_RELAY_GATE, spno_state);

}

void Relays::update(uint8_t *current_state) {
	switch(*current_state) {
    default:
    case STATE_STARTING_UP:
        dpco_state = DPCO_ON;
		spno_state = SPNO_OFF;
        break;

    case STATE_RUNNING:
        dpco_state = DPCO_ON;
		spno_state = SPNO_ON;
        break;

    case STATE_SHUTTING_DOWN:
        dpco_state = DPCO_ON;
		spno_state = SPNO_OFF;
        break;
    }
	
	/* --------- UPDATE DPCO STATE ---------- */
	
	
	/* --------- UPDATE SPNO STATE ---------- */
}
