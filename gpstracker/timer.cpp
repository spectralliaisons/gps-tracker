#include "timer.h"

Timer::Timer(int interval)
{
  // make sure first update fires timer
  _timer = -1;
  _intervalMs = interval;
  _initializing = true;
  _active = true;
}

bool Timer::update()
{
  if (!_active)
    return false;
  
  // if millis() or timer wraps around, we'll just reset it
  if (_timer > millis()) _timer = millis();

  if (millis() - _timer > _intervalMs || _initializing)
  {
    _initializing = false;
    
    reset();

    Serial.println("timer fired: " + String(millis() - _timer) + " vs " + String(_intervalMs));

    return true;
  }

  return false;
}

void Timer::reset()
{
  _timer = millis();
  _active = true;
}

void Timer::pause()
{
  _active = false;
}
