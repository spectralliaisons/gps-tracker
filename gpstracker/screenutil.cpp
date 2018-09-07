#include "Arduino.h"
#include "screenutil.h"
#include "pythagoras.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

#define BACKLIGHT_PIN 12 // analogWrite(12,0); // analogWrite(12,255);
#define BACKLIGHT_LEVEL_HI 15 // 3
#define BACKLIGHT_LEVEL_LO 3

#define NONE "??"

#define FEET_TO_PIXELS 0.1 // 0.05 zoom TODO: make editable
#define FEET_PER_MILE 5280

#define DEF_TEXT_SIZE 2
#define TEXT_PAD_Y 5
//#define BORDER_WIDTH 2

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS);

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

#define REQUIRE_TOUCH if (!_isTouched) return

bool toggle = true;

ScreenUtil::ScreenUtil(String initMsg)
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(BG);

  // initialize touchscreen
  if (!touch.begin())
    while (1);

  _batteryBottomPos = -999;
  _lastMsg = "";
  _isTouched = false;
  _lastTouchState = true; // force first _isTouched to work

  showMsg(initMsg);
}

/**
 * Handle touch state change.
 */
touch_state ScreenUtil::updateTouches()
{
  _isTouched = touch.touched();

  touch_state touchState = TouchState_noChange;

  // only respond to changes in touch state
  if (_lastTouchState == _isTouched)
    return touchState;

  bool newlyTouched = false;
  
  if (!_isTouched)
  {
    analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL_LO);
    
    // clear screen
    tft.fillScreen(BG);

    // force redraw battery
    _lastDisplayedCharge = "";

    touchState = TouchState_off;
  }
  else
  {
    analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL_HI);

    _lastDisplayedPosition = "";
    _lastMsg = "";
  
    uint16_t x, y;
    uint8_t z;
  
    int counter = 0;
    while (!touch.bufferEmpty()) {
  //    Serial.print("sweet touch " + String(counter++) + " is at " + String(touch.bufferSize()));
      
      touch.readData(&x, &y, &z);
      
      // Scale from ~0->4000 to tft.width using the calibration #'s
      point p;
      p.x = map(x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = map(y, TS_MINY, TS_MAXY, 0, tft.height());
      
  //    Serial.print("->("); 
  //    Serial.print(p.x); Serial.print(", "); 
  //    Serial.print(p.y); Serial.print(", "); 
  //    Serial.print(z);
  //    Serial.println(")");
  
  //    geoloc g = pointToGeoloc(p);
  
      String msg = "touched at: " + String(p.x) + ", " + String(p.y);
      
  //    showMsg(msg);
    }
    touch.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints 

    touchState = TouchState_on;
  }

  _lastTouchState = _isTouched;

  return touchState;
}

void ScreenUtil::showMsg(String msg)
{
//  Serial.println("msg: " + msg + " VS lastmsg: " + _lastMsg + " = " + String(msg == _lastMsg));
  
  if (msg == _lastMsg)
    return;

  _lastMsg = msg;
  
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

  // force all to redraw based on new window rect
//  _lastDisplayedCharge = "";
//  _lastDisplayedPosition = "";
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
  // to save power, only redraw if changed
  if (_lastDisplayedCharge != displayCharge)
  {
    println(_window.x, _batteryTopPos, DEF_TEXT_SIZE, isLow ? HX8357_RED : HX8357_GREEN, "[BATTERY] " + displayCharge + "%"); 
    _lastDisplayedCharge = displayCharge;
  }
}

void ScreenUtil::updateGPSText(Adafruit_GPS gps)
{
  REQUIRE_TOUCH;
  
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
  
  if (_lastDisplayedPosition != currPos)
  {
    _lastDisplayedPosition = currPos;
    
    println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*0, DEF_TEXT_SIZE, HX8357_WHITE, "lat: " + lat);
    println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*1, DEF_TEXT_SIZE, HX8357_WHITE, "lng: " + lng);
    println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*2, DEF_TEXT_SIZE, HX8357_WHITE, "altitude (feet): " + alt);
    println(left, top + (textHeightForSize(DEF_TEXT_SIZE) + TEXT_PAD_Y)*3, DEF_TEXT_SIZE, HX8357_WHITE, "satellites: " + sat);
    
//    drawBorder(gps.fix ? BG : HX8357_RED);
  }
}

//void ScreenUtil::drawBorder(int color)
//{
//  for (int i = 0; i < BORDER_WIDTH; i++)
//  {
//    tft.drawFastHLine(i, i, width()-(i*2), color);
//    tft.drawFastHLine(i, height()-(i+1), width(), color);
//    tft.drawFastVLine(i, i, height()-(i*2), color);
//    tft.drawFastVLine(width()-(i+1), i, height()-(i+1), color);
//  }
//}

// DISPLAY GPS COORDS

void ScreenUtil::updateGPSMap(String filePath)
{
  REQUIRE_TOUCH;
  
  File file = SD.open(filePath);
  
  if (!file)
    return;

  // clear screen where map appears
//  tft.fillRect(_window.x, _window.y, _window.width, _window.height, BG);

  uint32_t t0 = millis();
  
  int numPoints = 0;
  int numPointsOnscreen = 0;

  // get curr location (last on file)
  geoloc currGeoloc = findCurrGeoloc(filePath);

  drawDistancesFrom(currGeoloc);

  // geoloc one-back (when parsing)
  geoloc g1;
  g1.lat = 1/0; // stupid
  g1.lng = 1/0;

  // now go through again so we can compare all positions to curr pos for relative placement
  file = SD.open(filePath);
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

//  Serial.println();
//  Serial.println("parsed gps track in " + String(millis() - t0) + "ms");
  showMsg("parsed " + String(numPointsOnscreen) + "/" + String(numPoints) + " locations in " + String(millis() - t0) + "ms");
}

geoloc ScreenUtil::findCurrGeoloc(String filePath)
{
  File file = SD.open(filePath);
  
  String lastLine;
  geoloc currGeoloc;
  
  while (file.available()) 
  {
    lastLine = file.readStringUntil(' '); 
  }
  file.close();
  
  return Pythagoras::stringToGeoloc(lastLine);
}

/**
 * Concentric circles around curr loc to show distances from where you are
 */
void ScreenUtil::drawDistancesFrom(geoloc currGeoloc)
{
//  Serial.println("width " + String(_window.width) + " height " + String(_window.height));
  float maxFeet = pixelsToFeet(_window.width / 2);
//  Serial.println("maxFeet " + String(maxFeet));
  float feetPerRing = 500.0;
  float numRings = maxFeet / feetPerRing;
//  Serial.println("numRings " + String(numRings));

  for (int i = 1; i < numRings; i++)
  {
    float feet = i * feetPerRing;
    float r = feetToPixels(feet);
//    Serial.println("feet " + String(feet) + "r " + String(r));
    tft.drawCircle(_window.cx, _window.cy, r, HX8357_WHITE);
  }

  // legend
  tft.drawFastHLine(_window.cx - feetToPixels(feetPerRing), _window.cy, feetToPixels(feetPerRing), HX8357_WHITE);
  tft.setCursor(_window.cx - feetToPixels(feetPerRing) + 10, _window.cy - 10);
  tft.setTextSize(1);
  tft.setTextColor(HX8357_WHITE); 
  tft.println(String(round(feetPerRing)) + "ft");
}

bool ScreenUtil::drawGeoloc(geoloc g0, geoloc g1, geoloc currGeoloc)
{
//    String latStr = String(pos.lat, GpsUtil::precision());
//    String lngStr = String(pos.lng, GpsUtil::precision());
//    String posStr = "(" + latStr + ", " + lngStr + ")";
//    Serial.println("READ GPS POSITION FROM DISC: " + posStr);
    
    // only draw visible locations (gps coordinates that translate onto visible screen coords)
    point p0 = geolocToPoint(g0, currGeoloc);

    if (!pointIsOnscreen(p0))
      return false;
    else
      tft.fillCircle(p0.x, p0.y, 2, HX8357_BLUE);

    // assume we already drew a dot at this location because it was the last one we parsed!
    point p1 = geolocToPoint(g1, currGeoloc);

    if (pointIsOnscreen(p1))
      tft.drawLine(p1.x, p1.y, p0.x, p0.y, HX8357_WHITE); 

    return true;
}

bool ScreenUtil::pointIsOnscreen(point p)
{
  return (p.x >= _window.x && p.x <= _window.x + _window.width) && (p.y >= _window.y && p.y <= _window.y + _window.height);
}

float ScreenUtil::feetToPixels(float ft)
{
  return ft * FEET_TO_PIXELS;
}

float ScreenUtil::pixelsToFeet(float px)
{
  return px / FEET_TO_PIXELS;
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

//  Serial.println("positionToPoint ft " + String(ft) + " d " + String(d) + " r " + String(r) + " width " + String(_window.width) + " height " + String(_window.height) + " cx " + String(_window.cx) + " cy " + String(_window.cy));
  
  point p;
  p.x = _window.cx + d * sin(r);
  p.y = _window.cy + d * cos(r);
  
  return p;
}

geoloc ScreenUtil::pointToGeoloc(point pt)
{
    geoloc g;
    g.lat = 1/0; // TODO
    g.lng = 1/0;
    
    return g;
}

