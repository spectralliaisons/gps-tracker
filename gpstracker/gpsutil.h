#ifndef GpsUtil_h
#define GpsUtil_h

#include <Adafruit_GPS.h>

class GpsUtil
{
  public:
  	GpsUtil();
  	bool update();
  	Adafruit_GPS getGPS();
  private:
  	int _refreshMs;
    int _start;
};

#endif
