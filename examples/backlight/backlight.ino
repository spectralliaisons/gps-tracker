#include "Arduino.h"
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include<stdlib.h>
#include <SPI.h>

#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

bool toggle = true;

void setup() {
  Serial.begin(9600);
//  while (!Serial) {;}
  Serial.println("Serial initialized.");
  
  // put your setup code here, to run once:
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);

  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(HX8357_GREEN); 
  tft.println("BACKLIGHT TEST");

  Serial.println("BACKLIGHT TEST");
}

void loop() {
  // put your main code here, to run repeatedly:
  // BEGIN TEST BACKLIGHT

  tft.fillScreen(HX8357_BLACK);
  tft.setCursor(10, 10);
  
  if (toggle)
  {
    Serial.println("LITE OFF");
    tft.println("OFF");
    analogWrite(12,0);
  }
  else
  {
    Serial.println("LITE ON");
    tft.println("ON");
    analogWrite(12,5);
  }
  toggle = !toggle;
  // END TEST

  delay(500);
}
