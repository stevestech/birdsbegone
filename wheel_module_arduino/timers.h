#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include <Arduino.h>


// Timings
#define SHUTDOWN_DELAY					3000	// (ms)
#define SPI_WATCHDOG_LENGTH				3000	// (ms)

class Timers {
    private:
        bool gotFirstSpiPacket;
	unsigned long spi_watchdog_timer;
        bool *emergencyStop;
        
    public:
        Timers(bool *emergencyStop);
        void update(void);
        bool spi_polled;
};

#endif
