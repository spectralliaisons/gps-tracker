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
#define LOG_DELAY 60000//60000
#define LOG_NAME "battery.txt"
uint32_t logTimer = millis();

#define UPDATE_DELAY 10000
uint32_t updateTimer = millis();

Battery::Battery(int pin)
{
	_pin = pin;
  _start = millis();

  if (LOG_BATTERY)
    SDUtil::remove(LOG_NAME);
}

/**
 * Is it time to update the battery display?
 */
bool Battery::update()
{
  if (updateTimer > millis()) updateTimer = millis();

  if (millis() - updateTimer > UPDATE_DELAY)
  {
    updateTimer = millis();

    return true;
  }

  return false;
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
    if (logTimer > millis()) logTimer = millis();

    if (millis() - logTimer > LOG_DELAY)
    {
      logTimer = millis();

      // log voltage with timestamp
      String dataString = String(millis() - _start) + "," + String(v);
      SDUtil::print(LOG_NAME, dataString, true);
    }
  }
  
	_percentCharge = Pythagoras::scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, v); // voltage from 0-100
	
//	return String(v) + "V = " + String(_percentCharge);
  return String(int(round(_percentCharge)));
}

bool Battery::isLow()
{
  return _percentCharge <= WARN_PCT;
}

