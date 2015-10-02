#include <Arduino.h>

#include "battery.h"

Battery::Battery(void) {
	battery_24v_reading = 0;
	battery_12v_reading = 0;
	energy_consumed = 0;
	
	read_batteries = false;
	read_transducer = true;
}

void Battery::init()
{
    pinMode(CURRENT_SENSE, INPUT);
    pinMode(BATTERY_READ_1, INPUT);
    pinMode(BATTERY_READ_2, INPUT);
}

void Battery::update(uint8_t *current_state) {
	switch(*current_state) {
    default:
    case STATE_STARTING_UP:
        read_batteries = false;
		read_transducer = true;
        break;

    case STATE_RUNNING:
		read_transducer = true;
		read_transducer = true;
        break;

    case STATE_SHUTTING_DOWN:
        read_batteries = false;
		read_transducer = false;
        break;
	
	case STATE_EMERGENCY_STOP:
		read_batteries = true;
		read_transducer = true;
        break;
		
	case STATE_POWER_DOWN:
		read_batteries = false;
		read_transducer = false;
        break;
    } 
	
	/* --------- UPDATE BATTERY VOLTAGE READ ---------- */
	if (read_batteries == true)
	{
		// READ DA BATTERY VOLTAGE, NEED EQNS
	}
	
	/* --------- UPDATE CURRENT TRANSDUCER ---------- */
	if (read_transducer == true)
	{
		// READ IN THE TRANDUCER, DUNNO HOW TO DO THIS??
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
