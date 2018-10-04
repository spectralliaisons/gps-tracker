#include "Arduino.h"
#include "menu.h"
#include "pythagoras.h"
#include "sdutil.h"
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MIN 300
#define TS_MAX 3500

// Zoomed all the way in
#define MIN_DISPLAYABLE_FEET 50 // map window width is 50 feet

// Zoomed all the way out
#define MAX_DISPLAYABLE_FEET (25 * 5280) // map window width is 25 miles 

// Default zoom
#define DEF_DISPLAYABLE_FEET 1136.87

#define ZOOM_STEP 1.25

#define STMPE_CS 6
Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS);

Menu::Menu()
{
  // initialize touchscreen
  if (!touch.begin())
  {
    SDUtil::log("Menu::Menu: waiting for touchscreen...");
    while (1);
  }

  _state = MenuState_sleep;

  resetSwipeListener();

  _isTouched = false;
  _lastTouchState = false;

  _currMaxDisplayableFeet = DEF_DISPLAYABLE_FEET;
}

/**
 * Handle touch state change.
 */
menu_command Menu::getMenuCommand()
{
  _isTouched = touch.touched();

  point touchPoint = invalidTouchPoint();
  
  if (_isTouched)
  {
    touchPoint = getTouchPoint();

    // not sure how we can be touched but not have a valid touch point yet...
    if (!touchIsValid(touchPoint))
      return Menu_ignore;
  }

  // did touch state just change?
  bool touchStateDidNotChange = _isTouched == _lastTouchState;
  _lastTouchState = _isTouched;

  menu_command cmd = Menu_ignore;

  if (touchStateDidNotChange)
  {
    if (_isTouched)
    {
      // user still touching screen, so we'll track the touch point to check for a swipe when they release
      _touchOffX = touchPoint.x;

//      SDUtil::log("_touchOffX: " + String(_touchOffX));

      cmd = Menu_swiping;
    }
  }
  else // touch state changed...
  {
    if (_isTouched)
    {
      // touch just on
      _touchOnX = touchPoint.x;

//      SDUtil::log("_touchOnX: " + String(_touchOnX));
    } 
    else
    {
      // touch just released...
      
      // was this a swipe to toggle sleep/wake state?
      if (detectToggleSleep())
      {
        SDUtil::log("Menu::getMenuCommand : TOGGLE SLEEP/WAKE");
        
        if (_state == MenuState_sleep)
        {
          SDUtil::log("Menu::getMenuCommand : AWAKE");
          
          _state = MenuState_awake;
          cmd = Menu_wake;
        }
        else
        {
          SDUtil::log("Menu::getMenuCommand : SLEEP");
          
          _state = MenuState_sleep;
          cmd = Menu_sleep;
        }
      }
      else
      {
        // not a swipe... if we're awake, should we zoom?
        if (_state == MenuState_awake)
        {
          if (detectZoomIn())
          {
            zoomIn();
            cmd = Menu_zoom;
          }
          else if (detectZoomOut())
          {
            zoomOut();
            cmd = Menu_zoom;
          } 
        }
      }

      resetSwipeListener();
    }
  }
  
  return cmd;
}

bool Menu::touchIsValid(point p)
{
  return (p.x >= 0 && p.y >= 0);
}

point Menu::invalidTouchPoint()
{
  point p = {-1.0, -1.0};
  return p;
}

void Menu::resetSwipeListener()
{
  // reset swipe listener
  _touchOnX = _touchOffX = -1.0;
//  SDUtil::log("Menu::resetSwipeListener() _touchOnX = _touchOffX = " + String(_touchOnX));
}

bool Menu::detectToggleSleep()
{
  return (_touchOffX - _touchOnX) >= 0.5;
}

bool Menu::detectZoomIn()
{
  return _touchOnX < 0.5;
}

bool Menu::detectZoomOut()
{
  return _touchOnX >= 0.5;
}

void Menu::zoomIn()
{
  _currMaxDisplayableFeet = max(MIN_DISPLAYABLE_FEET, _currMaxDisplayableFeet / ZOOM_STEP);
  SDUtil::log("Menu::zoomIn() _currMaxDisplayableFeet: " + String(_currMaxDisplayableFeet));
}

void Menu::zoomOut()
{
  _currMaxDisplayableFeet = min(MAX_DISPLAYABLE_FEET, _currMaxDisplayableFeet * ZOOM_STEP);
  SDUtil::log("Menu::zoomOut() _currMaxDisplayableFeet: " + String(_currMaxDisplayableFeet));
}

float Menu::currMaxDisplayableFeet()
{
  return _currMaxDisplayableFeet;
}

void Menu::getTouchOnAndOffPoints(float *on, float *off, float *pct)
{
  *on = _touchOnX;
  *off = _touchOffX;
  *pct = max(0, min(1, (_touchOffX - _touchOnX) / 0.5));
}

/**
 * Return touched point with x, y each scaled from 0, 1
 */
point Menu::getTouchPoint()
{
  uint16_t x, y;
  uint8_t z;

  point touchPoint = invalidTouchPoint();

  while (!touch.bufferEmpty()) 
  {
    touch.readData(&x, &y, &z);
    
    // remember, map returns int, so value is rounded into bins
    // flipping x, y for landscape orientation
    // 0,0 = top left
    // 1,1 = bottom right
    touchPoint.y = Pythagoras::scale(TS_MAX, TS_MIN, 0, 1, x);
    touchPoint.x = Pythagoras::scale(TS_MIN, TS_MAX, 0, 1, y);

//    SDUtil::log("touch.readData: x: " + String(touchPoint.x) + ", y: " + String(touchPoint.y));
  }
  touch.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints

  return touchPoint;
}
