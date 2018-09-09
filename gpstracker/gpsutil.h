#ifndef GpsUtil_h
#define GpsUtil_h

#include <SD.h>
#include <Adafruit_GPS.h>
#include "timer.h"

class GpsUtil
{
  public:
  	GpsUtil();
  	bool update();
  	Adafruit_GPS getGPS();
    String getFilepath();
    static int precision();
  private:
    Timer *_timer;
    String _currLog;
    void logCurrentGeoloc();
};

#endif
