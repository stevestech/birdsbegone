#include <Arduino.h>

#include "buttons.h"

Buttons::Buttons() {
    // Init private variables
    start_flashing = false;
    stop_flashing = false;
    
    stop_pressed_reset = false;
    
    start_led_state = START_LED_OFF;
    prev_start_led_state = start_led_state;
    stop_led_state = STOP_LED_OFF;
    prev_stop_led_state = stop_led_state;
    prev_central_state = STATE_NULL;
    start_led_current_time = millis();
    stop_led_current_time = millis();
    stop_hold_time = millis();
	
    /*
     * Setup GPIO
     **/
}

void Buttons::init() {
  pinMode(STOP_READ, INPUT_PULLUP);
  pinMode(STOP_LED, OUTPUT);
  pinMode(START_LED, OUTPUT);
}

void Buttons::update(uint8_t *current_state) {
	/* ---------- UPDATE BUTTONS STATES DEPENDING CENTRAL ARDUINO STATE ---------- */
    // Swap only on state change
    if (*current_state != prev_central_state)
    {
   
    switch(*current_state) {
    default:
    case STATE_STARTING_UP:
        start_led_state = START_LED_ON;
	start_flashing = true;
	stop_led_state = STOP_LED_OFF;
	stop_flashing = false;
        break;

    case STATE_RUNNING:
        start_led_state = START_LED_ON;
        start_flashing = false;
        stop_led_state = STOP_LED_OFF;
	stop_flashing = false;
        break;

    case STATE_SHUTTING_DOWN:
        start_led_state = START_LED_OFF;
	start_flashing = false;
	stop_led_state = STOP_LED_ON;
	stop_flashing = true;
        break;
	
	case STATE_EMERGENCY_STOP:
        start_led_state = START_LED_ON;
	start_flashing = false;
	stop_led_state = STOP_LED_ON;
	stop_flashing = true;
        break;
	
	case STATE_POWER_DOWN:
        start_led_state = START_LED_OFF;
	start_flashing = false;
	stop_led_state = STOP_LED_OFF;
	stop_flashing = false;
        break;
    }
    prev_central_state = *current_state;
    }
	/* ---------- START LED UPDATE ---------- */
	// Update start LED on/off when state changes
	if (start_led_state != prev_start_led_state)
	{
		digitalWrite(START_LED, start_led_state);
		prev_start_led_state = start_led_state;
	}
	
	if (start_flashing == true)
	{
		if (millis() - start_led_current_time >= ((float)1/START_LED_FLASHING_RATE)*1000)
		{
                        
			start_led_current_time = millis();
			start_led_state = !start_led_state;
		}
	}
	/* ---------- STOP LED UPDATE ---------- */
	// Update stop LED on/off when state changes
	if (stop_led_state != prev_stop_led_state)
	{      
		digitalWrite(STOP_LED, stop_led_state);
		prev_stop_led_state = stop_led_state;
	}
	
	// Flash start led @ STOP_LED_FLASHING_RATE times per second
	if (stop_flashing == true)
	{
		if (millis() - stop_led_current_time >= ((float)1/STOP_LED_FLASHING_RATE)*1000)
		{
			stop_led_current_time = millis();
			stop_led_state = !stop_led_state;
		}
	}
	
	/* ---------- STOP READ ---------- */
	// When stop button is pressed, swap state to shutting down.

	if (!digitalRead(STOP_READ) == true)
	{
                stop_pressed_reset = true;
                stop_led_state = STOP_LED_ON;
                
                // Don't allow basic shutting down until RPi has booted and SPI talking
                if (*current_state != STATE_STARTING_UP)
                {
                  if (millis() - stop_hold_time >= STOP_HOLD_TIME)
                  {
  		    *current_state = STATE_SHUTTING_DOWN;
                  }
                }
                // Hard shutdown
                if (millis() - stop_hold_time >= STOP_HARD_RESET_TIME)
                {
		  *current_state = STATE_POWER_DOWN;
                }
	}
        else
        {
               if (stop_pressed_reset == true)
               {
                 stop_led_state = STOP_LED_OFF;
                 stop_pressed_reset = false;
               }
               stop_hold_time = millis();
        }
}
