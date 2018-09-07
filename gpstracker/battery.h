#ifndef Battery_h
#define Battery_h

#include "Arduino.h"

class Battery
{
  public:
    Battery(int pin);
    String displayCharge();
    bool isLow();
    bool update();
  private:
    float _percentCharge;
    int _pin;
    int _start;
};

#endif
