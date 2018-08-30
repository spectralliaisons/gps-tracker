#include "Arduino.h"
#include "battery.h"
#include "pythagoras.h"

#define VBAT_MAX 5.07 // 5.06 
#define VBAT_MIN 3.2

Battery::Battery(int pin)
{
	_pin = pin;
}

float Battery::percent()
{
	float charge = analogRead(_pin);
	charge *= 2;    // we divided by 2, so multiply back
	charge *= 3.3;  // Multiply by 3.3V, our reference voltage
	charge /= 1024; // convert to voltage
	charge = Pythagoras::scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, charge); // voltage from 0-100
	
	return charge;
}