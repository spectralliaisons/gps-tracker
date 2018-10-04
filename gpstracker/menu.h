#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include "pythagoras.h"

// TFT Touchscreen
#include "Adafruit_HX8357.h"

typedef enum {
  Menu_ignore,
  Menu_sleep,
  Menu_wake,
  Menu_zoom,
  Menu_swiping
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
    
    float currMaxDisplayableFeet();
    void getTouchOnAndOffPoints(float *on, float *off, float *pct);
   
   private:
    menu_state _state;
    
    float _touchOnX;
    float _touchOffX;

    float _currMaxDisplayableFeet;
    
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
