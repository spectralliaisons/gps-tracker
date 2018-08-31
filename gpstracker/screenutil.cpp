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
  tft.setCursor(0,120);
  tft.setTextColor(HX8357_GREEN); 
  tft.setTextSize(7);
  tft.println("bienvenidos");

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
}

int ScreenUtil::width()
{
  return tft.width();
}

int ScreenUtil::height()
{
  return tft.height();
}

void ScreenUtil::cls()
{
  tft.fillScreen(HX8357_BLACK);
}

void ScreenUtil::updateBatteryDisplay(String displayCharge, bool isLow)
{
  tft.setCursor(10, 15);
  tft.setTextColor(isLow ? HX8357_RED : HX8357_GREEN); 
  tft.setTextSize(2);  
  tft.println("[BATTERY] " + displayCharge + "%");
}

void ScreenUtil::println(int x, int y, int size, int color, String str)
{
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color); 
  tft.println(str);
}

void ScreenUtil::updateGPS(Adafruit_GPS gps)
{
  int left = 10;
  int top = 45;
    
  if (gps.fix) 
  {
    double alt = Pythagoras::cmToFeet(gps.altitude);

    println(left, top*1, 2, HX8357_WHITE, "lat: " + String(gps.latitudeDegrees, 10));
    println(left, top*2, 2, HX8357_WHITE, "lng: " + String(gps.longitudeDegrees, 10));
    println(left, top*3, 2, HX8357_WHITE, "altitude (feet): " + String(alt));
    println(left, top*4, 2, HX8357_WHITE, "satellites: " + String(gps.satellites));
  }
  else 
  {
    println(left, top, 2, WHITE, "WHERE MY SATELLITES AT");

    // red screen border
    tft.drawFastHLine(0, 0, width(), HX8357_RED);
    tft.drawFastHLine(0, height()-1, width(), HX8357_RED);
    tft.drawFastVLine(0, 0, height(), HX8357_RED);
    tft.drawFastVLine(width()-1, 0, height(), HX8357_RED);
    tft.drawFastHLine(1, 1, width()-2, HX8357_RED);
    tft.drawFastHLine(1, height()-2, width(), HX8357_RED);
    tft.drawFastVLine(1, 1, height()-2, HX8357_RED);
    tft.drawFastVLine(width()-2, 1, height()-2, HX8357_RED);
  }
}

