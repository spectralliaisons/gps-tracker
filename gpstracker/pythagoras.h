#ifndef Pythagoras_h
#define Pythagoras_h

#include "Arduino.h"

struct point
{
  float x;
  float y;
};

struct geoloc 
{
  float lat;
  float lng;
};

class Pythagoras
{
  public:
    static geoloc invalidGeoloc();
    static bool isValidGeoloc(geoloc g);
    static String geolocToString(geoloc g);
    static float cmToFeet(float cm);
    static float degToRad(float deg);
    static float scale(float x0, float x1, float y0, float y1, float x2);
    static geoloc stringToGeoloc(String str);
    static float getFeetBetweenGeolocs(geoloc p1, geoloc p2);
    static point rotate(point p, float r);
};

#endif
