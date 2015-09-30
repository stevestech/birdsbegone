#include <Arduino.h>

#include "battery.h"

Battery::Battery(void) {
	battery_24v_reading = analogRead(BATTERY_READ_1);
	battery_12v_reading = analogRead(BATTERY_READ_2);
	energy_consumed = analogRead(CURRENT_SENSE);
	
    /*
     * Setup GPIO
     **/
	pinMode(CURRENT_SENSE, INPUT);
    pinMode(BATTERY_READ_1, INPUT);
    pinMode(BATTERY_READ_2, INPUT);
}

void Battery::update(uint8_t *current_state) {
	if (*current_state == STATE_RUNNING)
	{
		// If in state running, continually update the battery readings:
		// battery 24v analog read + conversion
		// battery 12v analog read + conversion
		// battery current sense analog read + conversion
	}
	else
	{
		// else declare all variables null or zero or something? To show they are not being read atm
	}
}

double *Battery::getBattery24VReading(void) {
    return &battery_24v_reading;
}

double *Battery::getBattery12VReading(void) {
    return &battery_12v_reading;
}

double *Battery::getEnergyConsumed(void) {
    return &energy_consumed;
}