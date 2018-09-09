#ifndef Menu_h
#define Menu_h

#include <Adafruit_STMPE610.h>
#include "Arduino.h"
#include "pythagoras.h"
#include "timer.h"

typedef enum {
  TouchState_noChange = 0,
  TouchState_on = 1,
  TouchState_off = 2
} touch_state;

class Menu
{
  public:
    Menu(int pin);
    
    touch_state updateTouches();
    bool isTouched();
    float getFeetToPixelsByZoom();
  
  private:
    Adafruit_STMPE610 *_touch;
    Timer *_screenOffTimer;
    
    bool _isTouched;
    bool _lastTouchState;

    float _currZoom;
    float _lastZoom;
    
    bool isZoomUnchanged();
};

#endif
