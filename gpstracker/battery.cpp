#include "Arduino.h"
#include "battery.h"
#include "pythagoras.h"
#include "sdutil.h"

#define VBAT_MAX 5.05 // intermittently goes up to 5.07
#define VBAT_MIN 3.2
#define WARN_PCT 78 // 10

#define LOG_BATTERY true
#define LOG_DELAY 60000//60000
#define LOG_NAME "battery.txt"
uint32_t battTimer = millis();

Battery::Battery(int pin)
{
	_pin = pin;
  _start = millis();

  if (LOG_BATTERY)
    SDUtil::remove(LOG_NAME);
}

String Battery::displayCharge()
{
	float v = analogRead(_pin);
	v *= 2;    // we divided by 2, so multiply back
	v *= 3.3;  // Multiply by 3.3V, our reference voltage
	v /= 1024; // convert to voltage

  if (LOG_BATTERY)
  {
    // every once in a while log the battery life
    // if millis() or timer wraps around, we'll just reset it
    if (battTimer > millis()) battTimer = millis();

    if (millis() - battTimer > LOG_DELAY)
    {
      battTimer = millis();

      // log voltage with timestamp
      String dataString = String(millis() - _start) + "," + String(v);
      SDUtil::println(LOG_NAME, dataString);
    }
  }
  
	_percentCharge = Pythagoras::scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, v); // voltage from 0-100
	
	return String(v) + "V = " + String(_percentCharge);
}

bool Battery::isLow()
{
  return _percentCharge <= WARN_PCT;
}

