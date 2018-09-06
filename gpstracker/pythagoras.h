#ifndef Pythagoras_h
#define Pythagoras_h

#include "Arduino.h"

struct position 
{
  float lat;
  float lng;
};

class Pythagoras
{
  public:
    static float cmToFeet(float cm);
    static float degToRad(float deg);
    static float scale(float x0, float x1, float y0, float y1, float x2);
    static position stringToPosition(String str);
    static float getFeetBetweenPositions(position p1, position p2);
};

#endif
