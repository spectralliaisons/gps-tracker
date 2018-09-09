#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer
{
  public:
  	Timer(int interval);
  	bool update();
  	void reset();
  private:
  	int _intervalMs;
  	uint32_t _timer;
    bool _initializing;
};

#endif
