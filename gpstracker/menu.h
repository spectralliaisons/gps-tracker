#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include "pythagoras.h"
#include "timer.h"

// TFT Touchscreen
#include "Adafruit_HX8357.h"
#include <Adafruit_STMPE610.h>

typedef enum {
  Menu_ignore,
  Menu_updateZoom,
  Menu_touchOff,
  Menu_sleep,
  Menu_touched,
} menu_command;

class Menu
{
  public:
    Menu();
    menu_command getMenuCommand();
    bool isZoomUnchanged();
    bool isTouched();
    void startMenuTimer();
    float getFeetToPixelsByZoom();
   
   private:
   Timer *_sleepTimer;
   
    bool _isTouched;
    bool _lastTouchState;

    float _currZoom;
    float _lastZoom;
};

#endif
