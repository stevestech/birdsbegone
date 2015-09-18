#include <PID_v1.h>
#include <SPI.h>

#include "spiSlaveCentral.h"
#include "buttons.h"
#include "relays.h"
#include "battery.h"
#include "timers.h"
#include "central_states.h"

uint8_t central_arduino_state = STATE_STARTING_UP;

Buttons buttons;
Relays relays;
Battery battery;
Timers timers;
SpiSlave spiSlave(&battery, &timers);

void setup(void) {
    Serial.begin(9600);
    relays.init();
    buttons.init();
    battery.init();
    spiSlave.init();
}

void loop(void) {
        relays.update(&central_arduino_state);
	buttons.update(&central_arduino_state);
	battery.update(&central_arduino_state);
	spiSlave.update(&central_arduino_state);
        timers.update(&central_arduino_state);
}

// sudo service supervisor.sh status.start.stop
