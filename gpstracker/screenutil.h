#ifndef ScreenUtil_h
#define ScreenUtil_h

#include "Arduino.h"
#include "pythagoras.h"

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include <Adafruit_STMPE610.h>
#include "menu.h"

// GPS
#include <Adafruit_GPS.h>
#include "gpsutil.h"

#define BG HX8357_BLACK
#define WHITE HX8357_WHITE

typedef enum {
  Screen_ignore,
  Screen_drawBattery,
  Screen_drawMap
} screen_command;

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
    screen_command getScreenCommand();
    void updateSwipeDisplay();
    void updateZoomDisplay();
    void updateBatteryDisplay(String displayCharge, bool isLow);
    void println(int x, int y, int size, int color, String str, int bg=BG); // HX8357_BLACK // bg = HX8357_BLUE for debugging to see "text field" area
    void updateGPSText(Adafruit_GPS gps);
    void updateGPSDisplay(Adafruit_GPS gps, String trackFilepath, String mapFilepath);
    void drawGPSTrack(String filePath, geoloc currGeoloc);
    bool updateSDStatus(String filePath);
   
   private:
    Menu *_menu;
    
    rect _window;

    int _batteryTopPos;
    int _batteryBottomPos;

    int _sdStatusTopPos;
    int _sdStatusBottomPos;

    int textHeightForSize(int size);
    int textWidthForSize(int size);

    void setBacklightAndClearScreen(int level);
    void showMsg(String msg);

    geoloc findCurrGeoloc(String filePath);
    void drawDistancesFrom(geoloc currGeoloc);
    bool displayFeet();
    bool drawGeoloc(geoloc g0, geoloc g1, geoloc currGeoloc);
    bool pointIsOnscreen(point p);
    point geolocToPoint(geoloc pos, geoloc centerPos);
    geoloc pointToGeoloc(point pt);

    float feetToPixels(float feet);
    float pixelsToFeet(float px);

    void drawBmp(char *filename, uint16_t x, uint16_t y);
    uint16_t read16(File &f);
    uint32_t read32(File &f);
};

#endif
