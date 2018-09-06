#ifndef ScreenUtil_h
#define ScreenUtil_h

#include "Arduino.h"

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

// GPS
#include <Adafruit_GPS.h>
#include "gpsutil.h"

#define BG HX8357_BLACK
#define WHITE HX8357_WHITE

struct point
{
  int x;
  int y;
};

struct rect
{
  int x;
  int y;
  int width;
  int height;
};

class ScreenUtil
{
  public:
    ScreenUtil(String initMsg);
    void updateBatteryDisplay(String displayCharge, bool isLow);
    void println(int x, int y, int size, int color, String str, int bg=BG); // HX8357_BLACK // bg = HX8357_BLUE for debugging to see "text field" area
    void updateGPSText(Adafruit_GPS gps);
    void updateGPSMap(File file);
    void updateSDStatus(File file);
   
   private:
    rect _window;

    int _batteryTopPos;
    int _batteryBottomPos;
    
    int textHeightForSize(int size);
    int textWidthForSize(int size);
    
    void showMsg(String msg);
    
//    void drawBorder(int color);

    String _lastMsg;
    String _lastDisplayedCharge;
    String _lastDisplayedPosition;
    
    bool positionIsOnScreen(position pos);
    point positionToPoint(position pos);
};

#endif
