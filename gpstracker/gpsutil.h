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
    String getTrackFilepath();
    String getMapFilepath();
    static int precision();
  private:
    bool wasSentenceReceived();
    Timer *_timer;
    String _currTrack;
    String _currMap;
    void logCurrentGeoloc();
};

#endif
