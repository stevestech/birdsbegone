#include <Arduino.h>

#include "timers.h"

Timers::Timers(void) {
	shutdown_timer = millis();
	spi_watchdog_timer = millis();
	shutdown_timer_latch = false;
	spi_polled = false;
}

void Timers::update(uint8_t *current_state) {
	
	/* ---------- SHUTDOWN WATCHDOG ---------- */
        // Watches the SS line when shutting down, then turns off Arduino in SHUTDOWN_DELAY milliseconds after line goes low.
	if (*current_state == STATE_SHUTTING_DOWN)
	{
            if (digitalRead(SS) == LOW)
            {
                // Record current time
  	        if (shutdown_timer_latch == false)
  	        {
  	          shutdown_timer = millis();
                  shutdown_timer_latch = true;
                }
                
                // After SHUTDOWN_DELAY time has elapsed from recording of current time
  		if (millis() - shutdown_timer >= SHUTDOWN_DELAY)
  		{
  	          *current_state = STATE_POWER_DOWN;
  		}
            }
            else  { shutdown_timer_latch = false;  }
	}
	
	/* ---------- SPI WATCHDOG TIMER ---------- */
	
	if (*current_state == STATE_RUNNING)
	{
		// Recording every SPI poll, resetting the watchdog timer
		if (spi_polled == true)
		{
			spi_watchdog_timer = millis();
			spi_polled = false;
		}
		
		// After SPI_WATCHDOG_LENGTH time has elapsed from recording of current time
		if (millis() - spi_watchdog_timer >= SPI_WATCHDOG_LENGTH)
		{
			*current_state = STATE_EMERGENCY_STOP;
		}
	}
}
