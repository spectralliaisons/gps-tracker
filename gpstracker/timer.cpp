#include "timer.h"

Timer::Timer(String id, int interval)
{
  // make sure first update fires timer
  _timer = -1;
  _id = id;
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

   // SDUtil::log("Timer::" + _id + " fired");

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
