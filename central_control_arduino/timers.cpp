#include <Arduino.h>

#include "timers.h"

Timers::Timers(void) {
	shutdown_timer = millis();
	spi_watchdog_timer = millis();
	shutdown_timer_latch = false;
	spi_polled = false;
}

void Timers::update(uint8_t *current_state) {
	
	/* ---------- SHUTDOWN TIMER ---------- */
	// NOTE: Could potentially just check the RPi SPI lines to see if any lines are high until fully shut down
	// That way the Arduino will power off at the correct time every time!
	if (*current_state == STATE_SHUTTING_DOWN)
	{
		// Record current time
		if (shutdown_timer_latch == false)
		{
			shutdown_timer = millis();
			shutdown_timer_latch = true;
		}
		
		// After SHUTDOWN_LENGTH time has elapsed from recording of current time
		if (millis() - shutdown_timer >= SHUTDOWN_LENGTH)
		{
			*current_state = STATE_POWER_DOWN;
		}
	}
	
	/* ---------- SPI WATCHDOG TIMER ---------- */
	// NOTE: Need to set the spi_polled flag true somehow???! Could do this within spiSlaveCentral.cpp, but that
	//		does not seem very abstract, is there a way to check spi polls from this module to keep everything together?
	
	// Could be a bug when switching to this start with no spi polled, automatically goes to emergency stop
	// Is it possible to swap to this state without SPI being polled?
	
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
