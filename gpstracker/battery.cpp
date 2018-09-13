#include "Arduino.h"
#include "battery.h"
#include "pythagoras.h"
#include "sdutil.h"

// battery specs say the voltage goes down to 3.2, but based on my tests the values below indicate the observed range before death
// https://docs.google.com/spreadsheets/d/1li3eQBzjuMfZsQG_ix9UlOx6P3qg4OEhaA5oD-0nIow/edit#gid=179140147
#define VBAT_MAX 5.04 // intermittently goes up to 5.07
#define VBAT_MIN 4.6 // 3.2
#define WARN_PCT 10

#define LOG_BATTERY true
#define LOG_DELAY 10000
#define LOG_PREFIX "BAT"

#define UPDATE_DELAY 10000

Battery::Battery(int pin)
{
	_pin = pin;
  _start = millis();

  if (LOG_BATTERY)
  {
    _currLog = SDUtil::increment(LOG_PREFIX);
    SDUtil::remove(_currLog);

    // print headers
    SDUtil::print(_currLog, "secs,voltage,percent", true);
  }

  _logTimer = new Timer("LOG_BATTERY", LOG_DELAY);

  _updateTimer = new Timer("UPDATE_BATTERY", UPDATE_DELAY);
}

/**
 * Is it time to update the battery display?
 */
bool Battery::update()
{
  return _updateTimer->update();
}

String Battery::displayCharge()
{
	float v = analogRead(_pin);
	v *= 2;    // we divided by 2, so multiply back
	v *= 3.3;  // Multiply by 3.3V, our reference voltage
	v /= 1024; // convert to voltage

  _percentCharge = Pythagoras::scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, v); // voltage from 0-100

  if (LOG_BATTERY)
  {
    if (_logTimer->update())
    {
      // log voltage with timestamp
      int secsSinceLaunch = (millis() - _start) / 1000;
      String dataString = String(secsSinceLaunch) + "," + String(v) + "," + String(_percentCharge);
      SDUtil::print(_currLog, dataString, true);
    }
  }
	
//	return String(v) + "V = " + String(_percentCharge);
  return String(int(round(_percentCharge)));
}

bool Battery::isLow()
{
  return _percentCharge <= WARN_PCT;
}
