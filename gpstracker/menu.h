#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include "pythagoras.h"
//#include "timer.h"

// TFT Touchscreen
#include "Adafruit_HX8357.h"
#include <Adafruit_STMPE610.h>

typedef enum {
  TouchState_noChange,
  TouchState_on,
  TouchState_off
} touch_state;

class Menu
{
  public:
    Menu();
    touch_state updateTouches();
    bool isZoomUnchanged();
    bool isTouched();
    float getFeetToPixelsByZoom();
   
   private:
//   Timer *_sleepTimer;
   
    bool _isTouched;
    bool _lastTouchState;

    float _currZoom;
    float _lastZoom;
};

#endif
