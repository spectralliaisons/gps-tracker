#ifndef Battery_h
#define Battery_h

#include "Arduino.h"

class Battery
{
  public:
    Battery(int pin);
    float percent();
  private:
    int _pin;
};

#endif