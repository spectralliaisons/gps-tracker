#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include "pythagoras.h"

// TFT Touchscreen
#include "Adafruit_HX8357.h"
#include <Adafruit_STMPE610.h>

typedef enum {
  Menu_ignore,
  Menu_sleep,
  Menu_wake,
  Menu_zoom,
} menu_command;

typedef enum {
  MenuState_sleep,
  MenuState_awake
} menu_state;

class Menu
{
  public:
    Menu();
    
    menu_command getMenuCommand();
    
    float currFeetToPixels();
   
   private:
    menu_state _state;
    
    float _touchOnX;
    float _touchOffX;

    float _currFeetToPixels;
    
    bool _isTouched;
    bool _lastTouchState;

    bool touchIsValid(point p);

    point getTouchPoint();
    point invalidTouchPoint();
    
    bool detectToggleSleep();
    bool detectZoomIn();
    bool detectZoomOut();

    void zoomIn();
    void zoomOut();

    void resetSwipeListener();
};

#endif
