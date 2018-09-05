#ifndef ScreenUtil_h
#define ScreenUtil_h

#include "Arduino.h"

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

// GPS
#include <Adafruit_GPS.h>
#include "gpsutil.h"

#define WHITE HX8357_WHITE

struct point
{
  int x;
  int y;
};

class ScreenUtil
{
  public:
    ScreenUtil();
    void updateBatteryDisplay(String displayCharge, bool isLow);
    void println(int x, int y, int size, int color, String str);
    void updateGPSText(Adafruit_GPS gps);
    int updateGPSMap(File file);
   private:
    int width();
    int height();
    void drawBorder(int color);
    String _lastDisplayedCharge;
    String _lastDisplayedPosition;
    int _zoom;
    bool positionIsOnScreen(position pos);
    point positionToPoint(position pos);
};

#endif
