/**
 * 2018.08.22 Wes Jackson
 * wes@wesmjackson.com
 * 
 * First, make sure we have a port for the board:
 * Tools > Port > Feather M0
 */

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#ifdef ARDUINO_SAMD_FEATHER_M0
   #define STMPE_CS 6
   #define TFT_CS   9
   #define TFT_DC   10
   #define SD_CS    5
#endif

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
  Serial.println("initialized!");

  // initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);

  testText("1234567");
}

/**
 * Main program loop
 */
void loop() {

  
}

/**
 * Helper functions
 */

void testText(const char* str) {
  
  tft.fillScreen(HX8357_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_WHITE);  
  tft.setTextSize(5);
  tft.println(str);

  Serial.println(str);
}

