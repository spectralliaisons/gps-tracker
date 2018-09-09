#ifndef ScreenUtil_h
#define ScreenUtil_h

#include "Arduino.h"
#include "pythagoras.h"

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

// GPS
#include <Adafruit_GPS.h>
#include "gpsutil.h"

// Menu handles user interaction
#include "menu.h"

#define BG HX8357_BLACK
#define WHITE HX8357_WHITE

struct rect
{
  int x;
  int y;
  int width;
  int height;
  int cx;
  int cy;
};

class ScreenUtil
{
  public:
    ScreenUtil(String initMsg);
    touch_state updateTouches();
    void updateBatteryDisplay(String displayCharge, bool isLow);
    void println(int x, int y, int size, int color, String str, int bg=BG); // HX8357_BLACK // bg = HX8357_BLUE for debugging to see "text field" area
    void updateGPSText(Adafruit_GPS gps);
    void updateGPSMap(String filePath);
    bool updateSDStatus(String filePath);
   
   private:
    Menu *_menu;
    
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

    geoloc findCurrGeoloc(String filePath);
    void drawDistancesFrom(geoloc currGeoloc);
    bool drawGeoloc(geoloc g0, geoloc g1, geoloc currGeoloc);
    bool pointIsOnscreen(point p);
    point geolocToPoint(geoloc pos, geoloc centerPos);
    geoloc pointToGeoloc(point pt);

    float feetToPixels(float ft);
    float pixelsToFeet(float px);
};

#endif
