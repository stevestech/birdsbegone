#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include "timer.h"
#include "central_states"

// Timings
#define SHUTDOWN_LENGTH					30000	// (ms)
#define SPI_WATCHDOG_LENGTH				5000	// (ms)

class Timers {
    private:
		unsigned long shutdown_timer;
		unsigned long spi_watchdog_timer;
		bool shutdown_timer_latch;
		bool spi_polled;
        
    public:
        Timers(void);
        void update(uint8_t *current_state);
		
};

#endif