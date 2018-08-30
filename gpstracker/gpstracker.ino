/**
 * 2018.08.22 Wes Jackson
 * wes@wesmjackson.com
 * 
 * First, make sure we have a port for the board:
 * Tools > Port > Feather M0
 */

#include<stdlib.h>
#include <math.h>
#include <SPI.h>
#include "battery.h"
#include "pythagoras.h"

// battery pin for knowing battery life
// refer to: https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7
Battery battery = Battery(VBATPIN);

// GPS
#include <Adafruit_GPS.h>

// hardware serial port
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

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

uint32_t timer = millis();

/**
 * Initialize
 */
void setup() {
  
  // initialize console logging & wait for serial port to connect.
  Serial.begin(115200);
  while (!Serial) {;}
  Serial.println("Serial initialized.");

  // default NMEA GPS baud
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  // initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.println("initializing...");

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

  Serial.println("initialization complete.");
}

/**
 * Main program loop
 */
void loop() {

  // clear screen
//  tft.fillScreen(HX8357_BLACK);

//  updateBatteryDisplay();

  updateGPSDisplay();
  
//  delay(1000);
}

/**
 * Helper functions
 */

void cls() {

  tft.fillScreen(HX8357_BLACK);
}

void updateBatteryDisplay() {

  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_GREEN); 
  tft.setTextSize(2);

  float charge = battery.percent();

  // after analog read of battery pin, we must set it back to an input pin for the TFT display
  // https://forums.adafruit.com/viewtopic.php?f=57&t=139616&p=690795&hilit=TFT+analogRead#p690795 
  pinMode(VBATPIN, INPUT_PULLUP);
  
  tft.println("[BATTERY] " + String(charge) + "%");
}

void updateGPSDisplay() {

//    Serial.println("updateGPSDisplay()");

    // ensure GPS is read
    char c = GPS.read();

    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {

      Serial.println("GPS.newNMEAreceived YES");
      Serial.println(GPS.lastNMEA());

      if (!GPS.parse(GPS.lastNMEA()))
        return;
    }

    // if millis() or timer wraps around, we'll just reset it
    if (timer > millis()) timer = millis();

    // approximately every 2 seconds or so, print out the current stats
    if (millis() - timer > 2000) {
      timer = millis(); // reset the timer

      cls();
      updateBatteryDisplay();
      
      Serial.print("\nTime: ");
      Serial.print(GPS.hour, DEC); Serial.print(':');
      Serial.print(GPS.minute, DEC); Serial.print(':');
      Serial.print(GPS.seconds, DEC); Serial.print('.');
      Serial.println(GPS.milliseconds);
      Serial.print("Date: ");
      Serial.print(GPS.day, DEC); Serial.print('/');
      Serial.print(GPS.month, DEC); Serial.print("/20");
      Serial.println(GPS.year, DEC);
      Serial.print("Fix: "); Serial.print((int)GPS.fix);
      Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

      tft.setTextColor(HX8357_WHITE); 
      tft.setTextSize(2);
      tft.setCursor(0, 30);
      
      if (GPS.fix) {
        Serial.println("YES WE RECEIVED A FIX");
        Serial.print("FIX Location: ");
        Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
        Serial.print(", ");
        Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
        Serial.print("FIX Speed (knots): "); Serial.println(GPS.speed);
        Serial.print("FIX Angle: "); Serial.println(GPS.angle);
        Serial.print("FIX Altitude: "); Serial.println(GPS.altitude);
        Serial.print("FIX Satellites: "); Serial.println((int)GPS.satellites);
        
        double alt = Pythagoras::cmToFeet(GPS.altitude);
        
        tft.println("lat: " + String(GPS.latitudeDegrees, 10));
        tft.setCursor(0, 60);
        tft.println("lng: " + String(GPS.longitudeDegrees, 10));
        tft.setCursor(0, 90);
        tft.println("altitude (feet): " + String(alt));
        tft.setCursor(0, 120);
        tft.println("satellites: " + String(GPS.satellites));
      }
      else {
        Serial.println("NO FIX");
        tft.println("acquiring satellites...");
      }
    }
}

