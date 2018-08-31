#ifndef ScreenUtil_h
#define ScreenUtil_h

#include "Arduino.h"

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#define WHITE HX8357_WHITE

class ScreenUtil
{
  public:
    ScreenUtil();
    void cls();
    void updateBatteryDisplay(String displayCharge, bool isLow);
    void println(int x, int y, int size, int color, String str);
};

#endif
