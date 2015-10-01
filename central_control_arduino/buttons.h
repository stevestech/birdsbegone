#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include "central_states"

// Hub motor pins
#define STOP_LED              			2      	// D2
#define STOP_READ                       3      	// D3
#define START_LED						4      	// D4

// LED states
#define STOP_LED_OFF					0
#define STOP_LED_ON						1
#define START_LED_OFF					0
#define START_LED_ON					1

#define STOP_LED_FLASHING_RATE			4		// Times per second
#define START_LED_FLASHING_RATE			4		// Times per second

// Timings
#define SHUTDOWN_LENGTH					30000	// (ms)

class Buttons {
    private:
		bool start_flashing;
		bool stop_flashing;
		uint8_t start_led_state;
		uint8_t prev_start_led_state;
		uint8_t stop_led_state;
		uint8_t prev_stop_led_state;
		unsigned long start_led_current_time;
		unsigned long stop_led_current_time;
        
    public:
        Buttons(void);
        void update(uint8_t *current_state);
		
};


#endif