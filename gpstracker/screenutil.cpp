#include "Arduino.h"
#include "screenutil.h"
#include "pythagoras.h"
#include "sdutil.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

#define BACKLIGHT_PIN 12
#define BACKLIGHT_LEVEL_HI 255
#define BACKLIGHT_LEVEL_LO 3

#define NONE "??"

#define FEET_PER_MILE 5280

#define DEF_TEXT_SIZE 2
#define TEXT_PAD_Y 5

#define FEET_PER_MILE 5280
#define MIN_DISPLAYED_FEET (FEET_PER_MILE / 3)

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

bool toggle = true;

ScreenUtil::ScreenUtil(String initMsg)
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(BG);

  SDUtil::log("screen initialized. width: " + String(tft.width()) + " height: " + String(tft.height()));
  
  analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL_LO);

  _menu = new Menu();

  _batteryBottomPos = -999;

  showMsg(initMsg);
}

/**
 * Handle touch state change.
 */
screen_command ScreenUtil::getScreenCommand()
{
  menu_command menuCmd = _menu->getMenuCommand();
  
  screen_command screenCmd = Screen_ignore;

  switch (menuCmd)
  {
    case Menu_sleep:
      setBacklightAndClearScreen(BACKLIGHT_LEVEL_LO);
      
      showMsg("Swipe right to wake/sleep.");
      
      screenCmd = Screen_drawBattery;
      break;
      
    case Menu_zoom:
    case Menu_wake:
      setBacklightAndClearScreen(BACKLIGHT_LEVEL_HI);
      
      updateZoomDisplay();

      screenCmd = Screen_drawMap;
      break;
  } 

  return screenCmd;
}

void ScreenUtil::setBacklightAndClearScreen(int level)
{
  analogWrite(BACKLIGHT_PIN, level);
  tft.fillScreen(BG);
}

void ScreenUtil::showMsg(String msg)
{  
  println(0, 0, DEF_TEXT_SIZE, HX8357_WHITE, msg, HX8357_RED);

  // make way for the message
  _window.x = 0;
  _window.y = textHeightForSize(DEF_TEXT_SIZE);
  _window.width = tft.width();
  _window.height = tft.height() - _window.y;
  _window.cx = (_window.width)/2 + _window.x;
  _window.cy = (_window.height)/2 + _window.y;

  _batteryTopPos = _window.y + TEXT_PAD_Y;
  _batteryBottomPos = _batteryTopPos + textHeightForSize(DEF_TEXT_SIZE);
}

int ScreenUtil::textHeightForSize(int size)
{
  return size * 7;
}

int ScreenUtil::textWidthForSize(int size)
{
  return size * 6;
}

void ScreenUtil::println(int x, int y, int size, int color, String str, int bg)
{
  // clear bg for this text, assuming no line breaks!
  int width = _window.width - _window.x + x; // str.length() * textWidthForSize(size)
  tft.fillRect(x, y, width, textHeightForSize(size), bg);
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color); 
  tft.println(str);

  SDUtil::log("ScreenUtil::println: " + str);
}

bool ScreenUtil::updateSDStatus(String filePath)
{
  File file = SD.open(filePath);
  
  if (!file)
  {
    showMsg("ERROR: Could not load file from SD card");
    return false;
  }
  else
  {
//    showMsg("");
    return true;
  }
}

void ScreenUtil::updateBatteryDisplay(String displayCharge, bool isLow)
{
  println(_window.x, _batteryTopPos, DEF_TEXT_SIZE, isLow ? HX8357_RED : HX8357_GREEN, "[BATTERY] " + displayCharge + "%");
}

void ScreenUtil::updateGPSText(Adafruit_GPS gps)
{
  int left = _window.x;
  int top = _batteryBottomPos + TEXT_PAD_Y; // below battery text area

  String lat, lng, alt, sat;

  if (gps.fix) 
  {
    lat = String(gps.latitudeDegrees, 10);
    lng = String(gps.longitudeDegrees, 10);
    alt = Pythagoras::cmToFeet(gps.altitude);
    sat = String(gps.satellites);
  }
  else 
    lat = lng = alt = sat = NONE;

  String currPos = String(lat) + "_" + String(lng);
  
  println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*0, DEF_TEXT_SIZE, HX8357_YELLOW, "lat: " + lat);
  println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*1, DEF_TEXT_SIZE, HX8357_YELLOW, "lng: " + lng);
  println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*2, DEF_TEXT_SIZE, HX8357_YELLOW, "altitude (feet): " + alt);
  println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*3, DEF_TEXT_SIZE, HX8357_YELLOW, "satellites: " + sat);
}

// DISPLAY GPS COORDS

void ScreenUtil::updateGPSDisplay(String trackFilepath, String mapFilepath)
{
  uint32_t t0 = millis();
  
  // get curr location (last on file)
  geoloc currGeoloc = findCurrGeoloc(trackFilepath);

  SDUtil::log("currGeoloc: " + Pythagoras::geolocToString(currGeoloc));

  drawDistancesFrom(currGeoloc);

  drawGPSTrack(trackFilepath, currGeoloc);
  drawGPSTrack(mapFilepath, currGeoloc);

  SDUtil::log("Updated GPS display in " + String(millis() - t0) + "ms.");
}

void ScreenUtil::drawGPSTrack(String filePath, geoloc currGeoloc)
{
  uint32_t t0 = millis();
  
  File file = SD.open(filePath);
  if (!file)
    return;

  // geoloc one-back (when parsing)
  geoloc g1 = Pythagoras::invalidGeoloc();

  int numPoints = 0;
  int numPointsOnscreen = 0;

  // now go through again so we can compare all positions to curr pos for relative placement
  while (file.available()) 
  {
    String line = file.readStringUntil(' ');

    geoloc g0 = Pythagoras::stringToGeoloc(line);

    if (drawGeoloc(g0, g1, currGeoloc))
      numPointsOnscreen++;

    g1.lat = g0.lat;
    g1.lng = g0.lng;
    
    numPoints++;
  }
  file.close();

  showMsg("Rendered " + String(numPointsOnscreen) + "/" + String(numPoints) + " locations in " + String(millis() - t0) + "ms.");
}

geoloc ScreenUtil::findCurrGeoloc(String filePath)
{
  uint32_t t0 = millis();
  
  File file = SD.open(filePath);
  if (!file)
    return Pythagoras::invalidGeoloc();
  
  String lastLine;
  geoloc currGeoloc;
  
  while (file.available()) 
  {
    lastLine = file.readStringUntil(' '); 
  }
  file.close();
  
  geoloc g = Pythagoras::stringToGeoloc(lastLine);

  SDUtil::log("found current geoloc in " + String(millis() - t0) + "ms.");

  return g;
}

float ScreenUtil::feetToPixels(float feet)
{
  // _window.width                       px
  // -------------                     = -------------
  // _menu->currMaxDisplayableFeet()     feet
  //
  // => px * _menu->currMaxDisplayableFeet() = feet * _window.width
  // => px = (feet * _window.width) / _menu->currMaxDisplayableFeet()
  return (feet * _window.width) / _menu->currMaxDisplayableFeet();
}

float ScreenUtil::pixelsToFeet(float px)
{
  // _window.width                       px
  // -------------                     = -------------
  // _menu->currMaxDisplayableFeet()     feet
  //
  // => feet * _window.width = px * _menu->currMaxDisplayableFeet()
  // => feet = (px * _menu->currMaxDisplayableFeet()) / _window.width
  return (px * _menu->currMaxDisplayableFeet()) / _window.width;
}

bool ScreenUtil::displayFeet()
{
  return _menu->currMaxDisplayableFeet() < MIN_DISPLAYED_FEET;
}

void ScreenUtil::updateZoomDisplay()
{
  if (displayFeet())
    showMsg("ZOOM: " + String(pixelsToFeet(_window.width)) + " feet");
  else
    showMsg("ZOOM: " + String(pixelsToFeet(_window.width)/FEET_PER_MILE) + " miles");
}

/**
 * Concentric circles around curr loc to show distances from where you are
 */
void ScreenUtil::drawDistancesFrom(geoloc currGeoloc)
{
  float numRings = 6;
  float feetPerRing = (_menu->currMaxDisplayableFeet() / 2) / numRings;
  
  // octants
  int numRays = 32;
  float angle = 2 * PI / numRays; // number of dividing lines in 180-deg
  int lineLength = _window.width;
  for (int i = 0; i < numRays; i++)
  {
    // find this rotation on the unit circle
    float angle1 = i * angle;
    point vec0 = {1, 0}; 
    // (1, 0) on the unit circle is a horizontal line
    point vec1 = Pythagoras::rotate(vec0, angle1); // rotate to a new point on the unit circle

    // transform the point into window coordinates
    point p1;
    p1.x = _window.cx + vec1.x * lineLength;
    p1.y = _window.cy + vec1.y * lineLength;

    // now find corresponding 180-deg rotated point
    float angle2 = i * angle + PI;
    point vec2 = Pythagoras::rotate(vec0, angle2);
    point p2;
    p2.x = _window.cx + vec2.x * lineLength;
    p2.y = _window.cy + vec2.y * lineLength;
    
    tft.drawLine(p1.x, p1.y, p2.x, p2.y, HX8357_WHITE);
  }

  // clear center
  tft.fillCircle(_window.cx, _window.cy, feetToPixels(feetPerRing)-1, BG); 

  // draw concentric circles indicating distance from center
  for (int i = 1; i < numRings; i++)
  {
    float feet = i * feetPerRing;
    float r = feetToPixels(feet);
    tft.drawCircle(_window.cx, _window.cy, r, HX8357_WHITE);

    // legend
    if (i % 2 == 1)
    {
      // circles with legend are double-width
      tft.drawCircle(_window.cx, _window.cy, r+1, BG);
      tft.drawCircle(_window.cx, _window.cy, r+2, BG);
      tft.drawCircle(_window.cx, _window.cy, r+3, HX8357_WHITE);

      tft.setCursor(_window.cx + feetToPixels(feet) + 5, _window.cy - 10);
      tft.setTextSize(1);
      tft.setTextColor(HX8357_YELLOW); 

      if (displayFeet())
        tft.println("<-" + String(round(feet)) + "f"); 
      else
      {
        float mls = feet / FEET_PER_MILE;
        tft.println("<-" + String(mls) + "m");
      }
    }
  }
}

bool ScreenUtil::drawGeoloc(geoloc g0, geoloc g1, geoloc currGeoloc)
{
//    String latStr = String(pos.lat, GpsUtil::precision());
//    String lngStr = String(pos.lng, GpsUtil::precision());
//    String posStr = "(" + latStr + ", " + lngStr + ")";
//    SDUtil::log("READ GPS POSITION FROM DISC: " + posStr);
    
    // only draw visible locations (gps coordinates that translate onto visible screen coords)
    point p0 = geolocToPoint(g0, currGeoloc);

    if (!pointIsOnscreen(p0))
      return false;
    else
      tft.fillCircle(p0.x, p0.y, 3, HX8357_YELLOW);

    // assume we already drew a dot at this location because it was the last one we parsed!
    point p1 = geolocToPoint(g1, currGeoloc);

    if (pointIsOnscreen(p1))
      tft.drawLine(p1.x, p1.y, p0.x, p0.y, HX8357_YELLOW); 

    return true;
}

bool ScreenUtil::pointIsOnscreen(point p)
{
  return (p.x >= _window.x && p.x <= _window.x + _window.width) && (p.y >= _window.y && p.y <= _window.y + _window.height);
}

/**
 * Translate geolocation lng, lat to x, y s.t. the geolocation centerPos is at the center of the screen
 */
point ScreenUtil::geolocToPoint(geoloc pos, geoloc centerPos)
{
  // distance between these geopositions
  float ft = Pythagoras::getFeetBetweenGeolocs(pos, centerPos);
  float d = feetToPixels(ft);

  // angle between these
  float r = atan2(pos.lat - centerPos.lat, pos.lng - centerPos.lng);

  // we need to rotate 90deg ccw
  r += PI/2;

//  SDUtil::log("positionToPoint ft " + String(ft) + " d " + String(d) + " r " + String(r) + " width " + String(_window.width) + " height " + String(_window.height) + " cx " + String(_window.cx) + " cy " + String(_window.cy));
  
  point p;
  p.x = _window.cx + d * sin(r);
  p.y = _window.cy + d * cos(r);
  
  return p;
}

geoloc ScreenUtil::pointToGeoloc(point pt)
{
    geoloc g = Pythagoras::invalidGeoloc();
    
    return g;
}
