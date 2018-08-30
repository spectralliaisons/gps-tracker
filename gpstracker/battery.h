#ifndef Battery_h
#define Battery_h

#include "Arduino.h"

class Battery
{
  public:
    Battery(int pin);
    String getCharge();
  private:
    int _pin;
    int _start;
};

#endif
