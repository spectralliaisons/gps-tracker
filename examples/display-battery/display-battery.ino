/**
 * 2018.08.22 Wes Jackson
 * wes@wesmjackson.com
 * 
 * First, make sure we have a port for the board:
 * Tools > Port > Feather M0
 * 
 * Test displaying battery to TFT Featherwing
 */

#include <SPI.h>

// battery pin for knowing battery life
// refer to: https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7
#define VBAT_MAX 5.07 // 5.06 
#define VBAT_MIN 3.2

// TFT Touchscreen
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

/**
 * Initialize
 */
void setup() {
  
  // initialize console logging & wait for serial port to connect.
  Serial.begin(9600);
  while (!Serial) {;}
  tft.println("Serial ready.");

  // initialize TFT screen
  tft.begin(HX8357D);

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
  
  Serial.println(F("Benchmark                Time (microseconds)"));
  
  tft.setRotation(1);
}

/**
 * Main program loop
 */
void loop() {

  updateBatteryDisplay();
  
  delay(1000);
}

void updateBatteryDisplay() {

  tft.fillScreen(HX8357_BLACK);
  
  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_GREEN); tft.setTextSize(2);
//  tft.println(1234.56);
  tft.println("[BATTERY] " + String(getBatteryPercent()) + "%");
}

int getBatteryPercent() {

//  float charge = analogRead(VBATPIN);
  float charge = 560;
  charge *= 2;    // we divided by 2, so multiply back
  charge *= 3.3;  // Multiply by 3.3V, our reference voltage
  charge /= 1024; // convert to voltage
  charge = round(scale(VBAT_MIN, VBAT_MAX, 0.0, 100.0, charge)); // voltage from 0-100
//  return charge;

  return charge;
}

/**
 * Helper functions
 */

// clear screen
void cls() {

  tft.fillScreen(HX8357_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_WHITE);  
  tft.setTextSize(2);
}

// scale a value (x2) that ranges from (x0, x1) to (y0, y1)
float scale(float x0, float x1, float y0, float y1, float x2) {
    
    return max(y0, min(y1, (( x2 - x0 ) / ( x1 - x0 )) * ( y1 - y0 ) + y0));
}

