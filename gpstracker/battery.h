#ifndef Battery_h
#define Battery_h

#include "Arduino.h"
#include "timer.h"

class Battery
{
  public:
    Battery(int pin);
    String displayCharge();
    bool isLow();
    bool update();
  private:
  	Timer *_logTimer;
  	Timer *_updateTimer;
    float _percentCharge;
    int _pin;
    int _start;
};

#endif
