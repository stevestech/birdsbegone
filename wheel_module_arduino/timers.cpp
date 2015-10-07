#include <Arduino.h>

#include "timers.h"

Timers::Timers(bool *emergencyStop) {
        gotFirstSpiPacket = false;
	spi_watchdog_timer = millis();
	spi_polled = false;
        gotFirstSpiPacket = false;
        this->emergencyStop = emergencyStop;
}

void Timers::update(void) {
	/* ---------- SPI WATCHDOG TIMER ---------- */
	if (gotFirstSpiPacket)
	{
		// Recording every SPI poll, resetting the watchdog timer
		if (spi_polled)
		{
			spi_watchdog_timer = millis();
			spi_polled = false;
		}
		
		// After SPI_WATCHDOG_LENGTH time has elapsed from recording of current time
		if (millis() - spi_watchdog_timer >= SPI_WATCHDOG_LENGTH)
		{
			*emergencyStop = true;
		}
	}
}
