#include "Arduino.h"
#include "battery.h"
#include "pythagoras.h"

#define VBAT_MAX 5.05 // intermittently goes up to 5.07
#define VBAT_MIN 3.2

Battery::Battery(int pin)
{
	_pin = pin;
  _start = millis();
}

String Battery::getCharge()
{
	float v = analogRead(_pin);
	v *= 2;    // we divided by 2, so multiply back
	v *= 3.3;  // Multiply by 3.3V, our reference voltage
	v /= 1024; // convert to voltage

//  Serial.println(String(millis() - _start) + "," + String(charge));

  Serial.println(v);
  Serial.println(String(millis() - _start) + "," + String(int(round(v))));
  
	float pct = Pythagoras::scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, v); // voltage from 0-100
	
	return String(v) + "V = " + String(pct);
}
