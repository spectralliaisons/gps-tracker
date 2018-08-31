#include "Arduino.h"
#include "screenutil.h"

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

void ScreenUtil::cls()
{
  tft.fillScreen(HX8357_BLACK);
}

void ScreenUtil::updateBatteryDisplay(String displayCharge, bool isLow)
{
  tft.setCursor(5, 5);
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

