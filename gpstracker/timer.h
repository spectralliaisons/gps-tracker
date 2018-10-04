#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer
{
  public:
  	Timer(String id, int interval);
  	bool update();
  	void reset();
    void pause();
  private:
    String _id;
    bool _active;
  	int _intervalMs;
  	uint32_t _timer;
    bool _initializing;
};

#endif
