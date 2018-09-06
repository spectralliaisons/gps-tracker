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
#define BACKLIGHT_LEVEL 10

#define NONE "??"

#define DEF_ZOOM 10

#define DEF_TEXT_SIZE 2
#define TEXT_PAD_Y 5
//#define BORDER_WIDTH 2

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

bool toggle = true;

ScreenUtil::ScreenUtil(String initMsg)
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(BG);

  analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL);

  _batteryBottomPos = -999;
  _lastMsg = "";

  showMsg(initMsg);
}

void ScreenUtil::showMsg(String msg)
{
//  Serial.println("msg: " + msg + " VS lastmsg: " + _lastMsg + " = " + String(msg == _lastMsg));
  
  if (msg == _lastMsg)
    return;

  _lastMsg = msg;
  
  println(0, 0, DEF_TEXT_SIZE, HX8357_WHITE, msg, HX8357_RED);

  // make way for the message
  _window.y = textHeightForSize(DEF_TEXT_SIZE);
  _window.x = 0;
  _window.width = tft.width();
  _window.height = tft.height() - _window.y;

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

void ScreenUtil::updateSDStatus(File file)
{
  if (!file)
  {
    showMsg("ERROR: Could not load file from SD card");
  }
  else
  {
    showMsg("");
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

void ScreenUtil::updateGPSMap(File file)
{
  if (!file)
    return;

  uint32_t t0 = millis();
  
  // read from the file until there's nothing else in it:
  int numPoints = 0;
  int numPointsOnscreen = 0;
  while (file.available()) 
  {
    String line = file.readStringUntil(' ');

//    Serial.println(line);
    
    position pos = GpsUtil::stringToPosition(line);
    
    String latStr = String(pos.lat, GpsUtil::precision());
    String lngStr = String(pos.lng, GpsUtil::precision());
    String posStr = "(" + latStr + ", " + lngStr + ")";
//    Serial.println("READ GPS POSITION FROM DISC: " + posStr);
//
//    // only draw visible locations (gps coordinates that translate onto visible screen coords)
//    if (positionIsOnScreen(pos))
//    {
//      println(10, 45 + numPointsOnscreen*2*7, 2, HX8357_WHITE, posStr);
//      numPointsOnscreen++;
//    }
    numPoints++;
  }
  
  // close the file
  file.close();

  Serial.println();
  Serial.println("parsed gps track in " + String(millis() - t0) + "ms");
  showMsg("read " + String(numPoints) + " locations in " + String(millis() - t0) + "ms");
}

bool ScreenUtil::positionIsOnScreen(position pos)
{
  point p = positionToPoint(pos);
  return (_window.x <= p.x <= _window.width) && (_window.y <= p.y <= _window.height);
}

point ScreenUtil::positionToPoint(position pos)
{
  // use _zoom
  point p;
  p.x = _window.width / 2;
  p.y = _window.height / 2;
  return p;
}

