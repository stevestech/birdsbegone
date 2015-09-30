#include <Arduino.h>

#include "timer.h"
#include "buttons.h"

Buttons::Buttons(void) {
	// Init private variables
	start_flashing = true;
	
    start_led_state = START_LED_OFF;
	prev_start_led_state = start_led_state;
	stop_led_state = STOP_LED_OFF;
	prev_stop_led_state = stop_led_state;
	
	start_current_time = millis();
    /*
     * Setup GPIO
     **/
    pinMode(STOP_LED, OUTPUT);
    pinMode(STOP_READ, INPUT);
    pinMode(START_LED, OUTPUT);
    
    digitalWrite(STOP_LED, LOW);
    digitalWrite(START_LED, LOW);
}

void Buttons::update(uint8_t *current_state) {
	/* ---------- UPDATE BUTTONS STATES DEPENDING CENTRAL ARDUINO STATE ---------- */
	switch(*current_state) {
    default:
    case STATE_STARTING_UP:
        start_led_state = START_LED_ON;
		start_flashing = true;
		stop_led_state = STOP_LED_OFF;
        break;

    case STATE_RUNNING:
        start_led_state = START_LED_ON;
        start_flashing = false;
        stop_led_state = STOP_LED_OFF;
        break;

    case STATE_SHUTTING_DOWN:
        start_led_state = START_LED_OFF;
		start_flashing = true;
		stop_led_state = STOP_LED_ON;
        break;
    }    
	
	/* ---------- START LED UPDATE ---------- */
	
	// Update start LED on/off when state changes
	if (start_led_state != prev_start_led_state)
	{
		digitalWrite(START_LED, start_led_state);
		prev_start_led_state = start_led_state;
	}
	
	// Flash start led @ START_LED_FLASHING_RATE times per second
	if (start_flashing == true)
	{
		if (millis() - start_current_time >= (1/START_LED_FLASHING_RATE)*1000)
		{
			start_current_time = millis();
			start_led_state = !start_led_state;
		}
	}
	
	/* ---------- STOP LED UPDATE ---------- */
	
	// Update stop LED on/off when state changes
	if (stop_led_state != prev_stop_led_state)
	{
		digitalWrite(START_LED, stop_led_state);
		prev_stop_led_state = stop_led_state;
	}
	
	/* ---------- STOP READ ---------- */
	
	// When stop button is pressed, swap state to shutting down.
	// NEED TO CHECK IF CAN JUST DIGITAL READ THE GPIO PORT!
	// POTENTIALLY PLACE THIS ON AN INTERRUPT?
	if (digitalRead(STOP_READ) == true)
	{
		*current_state = STATE_SHUTTING_DOWN; 
	}
}
