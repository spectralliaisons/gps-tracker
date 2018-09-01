#include "Arduino.h"
#include "screenutil.h"
#include "pythagoras.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

ScreenUtil::ScreenUtil()
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);

  _lastDisplayedCharge = "";
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

void ScreenUtil::updateGPS(Adafruit_GPS gps)
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

    drawBorder(HX8357_BLACK);
  }
  else 
  {
    lat = lng = alt = sat = "??";

    drawBorder(HX8357_RED);
  }

  println(left, top, 2, HX8357_WHITE, "lat: " + lat);
  println(left, top*2, 2, HX8357_WHITE, "lng: " + lng);
  println(left, top*3, 2, HX8357_WHITE, "altitude (feet): " + alt);
  println(left, top*4, 2, HX8357_WHITE, "satellites: " + sat);
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

