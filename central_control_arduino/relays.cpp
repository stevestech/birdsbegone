#include <Arduino.h>

#include "relays.h"

Buttons::Buttons(void) {
    SPI_recieved = false;
	shutting_down = false;
    
    /*
     * Setup GPIO
     **/
    pinMode(STOP_LED, OUTPUT);
    pinMode(STOP_READ, INPUT);
    pinMode(START_LED, OUTPUT);
    
    digitalWrite(STOP_LED, LOW);
    digitalWrite(START_LED, LOW);
}

void Buttons::update(void) {
	
	// Update start LED button status
	if (SPI_recieved)
	{
		digitalWrite(START_LED, HIGH);
	}
	else
	{
		// FLASH START LED USING TIMERS
	}
	
	// UPDATE RED LED STATUS
	if (shutting_down)
	{
		digitalWrite(START_LED, HIGH);
	}
	
	// READ RED BUTTON STATUS (USE INTERRUPTS, OR POLL??)
	// WHEN TRIGGERED, SEND SHUTDOWN COMMAND TO RPI, THEN SHUTDOWN AFTER 30 SECONDS
}

/*
void HubMotor::setThrottle(uint8_t *newThrottle) {
    throttle = *newThrottle;
}


void HubMotor::setState(uint8_t *newState) {
    if ((*newState == STATE_NEUTRAL) ||
        (*newState == STATE_BRAKING) ||
        (*newState == STATE_FORWARD) ||
        (*newState == STATE_REVERSE)) {
            
        state = *newState;
    }
    
    else {
        state = STATE_NEUTRAL;
    }
}
*/
