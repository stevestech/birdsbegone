#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include "central_states"

// Hub motor pins
#define STOP_LED              			5      	// D5
#define STOP_READ                       6      	// D6
#define START_LED						7      	// D7

// LED states
#define STOP_LED_OFF					0
#define STOP_LED_ON						1
#define START_LED_OFF					0
#define START_LED_ON					1

#define STOP_LED_FLASHING_RATE			4		// Times per second (currently unused)
#define START_LED_FLASHING_RATE			4		// Times per second

class Buttons {
    private:
		bool start_flashing;
		uint8_t start_led_state;
		uint8_t prev_start_led_state;
		uint8_t stop_led_state;
		uint8_t prev_stop_led_state;
		unsigned long start_current_time;
        
    public:
        Buttons(void);
        void update(uint8_t *current_state);
		
};


#endif