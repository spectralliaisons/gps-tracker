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

#define NONE "??";

#define DEF_ZOOM 10;

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

bool toggle = true;

ScreenUtil::ScreenUtil()
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);

  analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL);

  _lastDisplayedCharge = NONE;
  _lastDisplayedPosition = NONE;

  _zoom = DEF_ZOOM;
}

int ScreenUtil::width()
{
  return tft.width();
}

int ScreenUtil::height()
{
  return tft.height();
}

void ScreenUtil::updateBatteryDisplay(String displayCharge, bool isLow)
{
  // to save power, only redraw if changed
  if (_lastDisplayedCharge != displayCharge)
  {
    println(10, 15, 2, isLow ? HX8357_RED : HX8357_GREEN, "[BATTERY] " + displayCharge + "%"); 
    _lastDisplayedCharge = displayCharge;
  }
}

void ScreenUtil::println(int x, int y, int size, int color, String str)
{
  tft.fillRect(x, y, width()-4-x, size*7, HX8357_BLACK); // HX8357_BLUE for debugging to see "text field" area
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color); 
  tft.println(str);
}

void ScreenUtil::updateGPSText(Adafruit_GPS gps)
{
  int left = 10;
  int top = 45;

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
    
    println(left, top, 2, HX8357_WHITE, "lat: " + lat);
    println(left, top*2, 2, HX8357_WHITE, "lng: " + lng);
    println(left, top*3, 2, HX8357_WHITE, "altitude (feet): " + alt);
    println(left, top*4, 2, HX8357_WHITE, "satellites: " + sat);
    
    drawBorder(gps.fix ? HX8357_BLACK : HX8357_RED);
  }
}

void ScreenUtil::drawBorder(int color)
{
  tft.drawFastHLine(0, 0, width(), color);
  tft.drawFastHLine(0, height()-1, width(), color);
  tft.drawFastVLine(0, 0, height(), color);
  tft.drawFastVLine(width()-1, 0, height(), color);
  tft.drawFastHLine(1, 1, width()-2, color);
  tft.drawFastHLine(1, height()-2, width(), color);
  tft.drawFastVLine(1, 1, height()-2, color);
  tft.drawFastVLine(width()-2, 1, height()-2, color);
}

// DISPLAY GPS COORDS

// returns number of bytes written to gps track on sd card
int ScreenUtil::updateGPSMap(File file)
{
  int bytesWritten = 0;
  
  if (!file)
    return bytesWritten;

  Serial.println("updateGPSMap() BEGIN");
  
  // read from the file until there's nothing else in it:
  int numPointsOnscreen = 0;
  while (file.available()) 
  {
    // read one byte at a time
    // file:///Applications/Arduino.app/Contents/Java/reference/www.arduino.cc/en/Serial/Read.html
    int line = file.read();

    bytesWritten += Serial.write(line);
    
//    position pos = GpsUtil::stringToPosition(line);
//    position pos;
//    pos.lat = 0;
//    pos.lng = 0;
//
//    String latStr = String(pos.lat, 10);
//    String lngStr = String(pos.lng, 10);
//    String posStr = "(" + latStr + ", " + lngStr + ")";
//    Serial.println("GPS POSITION FROM DISC: " + posStr);
//
//    // only draw visible locations (gps coordinates that translate onto visible screen coords)
//    if (positionIsOnScreen(pos))
//    {
//      println(10, 45 + numPointsOnscreen*2*7, 2, HX8357_WHITE, posStr);
//      numPointsOnscreen++;
//    }
  }
  
  // close the file
  file.close();

  Serial.println("updateGPSMap() END");

  return bytesWritten;
}

bool ScreenUtil::positionIsOnScreen(position pos)
{
  point p = positionToPoint(pos);
  return (0 <= p.x <= width()) && (0 <= p.y <= height());
}

point ScreenUtil::positionToPoint(position pos)
{
  // use _zoom
  point p;
  p.x = width() / 2;
  p.y = height() / 2;
  return p;
}

