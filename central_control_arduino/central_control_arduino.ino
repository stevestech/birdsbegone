#include <PID_v1.h>
#include <SPI.h>

#include "spiSlaveCentral.h"
#include "buttons.h"
#include "relays.h"
#include "battery.h"
#include "central_states"

SpiSlave spiSlave();
Buttons buttons();
Relays relays();
Battery battery();

void setup(void) {
    Serial.begin(9600);
	uint8_t central_arduino_state = STATE_STARTING_UP;
}

void loop(void) {
	buttons.update(&central_arduino_state);
	relays.update(&central_arduino_state);
	battery.update(&central_arduino_state);
	spiSlave.update(&central_arduino_state);
}