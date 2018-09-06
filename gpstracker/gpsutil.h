#ifndef GpsUtil_h
#define GpsUtil_h

#include <SD.h>
#include <Adafruit_GPS.h>

class GpsUtil
{
  public:
  	GpsUtil();
  	bool update();
  	Adafruit_GPS getGPS();
    File getFileFromDisc();
    static int precision();
  private:
  	int _refreshMs;
    int _start;
    String _currLog;
    void logCurrentPosition();
};

#endif
