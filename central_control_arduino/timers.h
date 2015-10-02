#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include <Arduino.h>
#include "central_states.h"


// Timings
#define SHUTDOWN_DELAY					3000	// (ms)
#define SPI_WATCHDOG_LENGTH				3000	// (ms)

class Timers {
    private:
	unsigned long shutdown_timer;
	unsigned long spi_watchdog_timer;
	bool shutdown_timer_latch;
        
    public:
        Timers(void);
        void update(uint8_t *current_state);
        bool spi_polled;
		
};

#endif
