
#include "menu.h"

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 300
#define TS_MAXX 3500

#define MIN_ZOOM 1
#define MAX_ZOOM 8 // 10

#define MIN_FEET_TO_PIXELS 0.09 // 0.1 // 0.05
#define MAX_FEET_TO_PIXELS 0.30

// screen off of untouched for long enough
#define DELAY_SCREEN_OFF 7000

Menu::Menu(int pin)
{
  _touch = new Adafruit_STMPE610(pin);

  // initialize touchscreen
  if (!_touch->begin())
    while (1);

  Timer *_screenOffTimer = new Timer(DELAY_SCREEN_OFF);

  _isTouched = false;
  _lastTouchState = true; // force first _isTouched to work
  _currZoom = _lastZoom = 0;

  Serial.println("menu initialized.");
}

bool Menu::isTouched()
{
  return _isTouched;
}

float Menu::getFeetToPixelsByZoom()
{
  return Pythagoras::scale(MIN_ZOOM, MAX_ZOOM, MIN_FEET_TO_PIXELS, MAX_FEET_TO_PIXELS, _currZoom);
}

touch_state Menu::updateTouches()
{
  _isTouched = _touch->touched();

  bool touchStateDidNotChange = _isTouched == _lastTouchState;
  _lastTouchState = _isTouched;

  touch_state touchState = TouchState_noChange;
  
  if (!_isTouched)
  {
    if (_screenOffTimer->update())
    {
      touchState = TouchState_off;
    }
  }
  else
  {
    // kill the screen off timer
    _screenOffTimer->reset();

    // maybe change zoom level (if so, force redraw map at this zoom)
    bool zoomIsSame = isZoomUnchanged();
    if (!touchStateDidNotChange || !zoomIsSame)
    {
      touchState = TouchState_on; 
    }
  }

  return touchState;
}

bool Menu::isZoomUnchanged()
{
  bool zoomDidNotChange = true;
  
  uint16_t x, y;
  uint8_t z;

  int counter = 0;
  while (!_touch->bufferEmpty()) 
  {
    _touch->readData(&x, &y, &z);
    
    // remember, map returns int, so value is rounded into bins
    _currZoom = map(x, TS_MINX, TS_MAXX, MIN_ZOOM, MAX_ZOOM);

    // only redraw if zoom changed
    if (_currZoom != _lastZoom)
    {
      zoomDidNotChange = false;
      Serial.println("------- ZOOM " + String(_currZoom));
      _lastZoom = _currZoom;
    }
  }
  _touch->writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints

  return zoomDidNotChange;
}
