#ifndef GpsUtil_h
#define GpsUtil_h

#include <SD.h>
#include <Adafruit_GPS.h>

struct position 
{
  float lat;
  float lng;
};

class GpsUtil
{
  public:
  	GpsUtil();
  	bool update();
  	Adafruit_GPS getGPS();
    File getFileFromDisc();
    static position stringToPosition(String str);
    static int precisionRead();
    static int precisionWrite();
    static float getFeetBetweenPositions(position p1, position p2);
  private:
  	int _refreshMs;
    int _start;
    String _currLog;
    void logCurrentPosition();
};

#endif
