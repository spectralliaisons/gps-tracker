/**
 * 2018.08.22 Wes Jackson
 * wes@wesmjackson.com
 * 
 * First, make sure we have a port for the board:
 * Tools > Port > Feather M0
 */

#include<stdlib.h>
#include <SPI.h>
#include "battery.h"
#include "gpsutil.h"
#include "screenutil.h"
#include "pythagoras.h"

// battery pin for knowing battery life
// refer to: https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7
Battery battery = Battery(VBATPIN);

// hardware serial port for GPS
GpsUtil *gps;

// whater display we are using
ScreenUtil *screen;

/**
 * Initialize
 */
void setup() 
{  
  // initialize console logging & wait for serial port to connect.
  Serial.begin(115200);
  while (!Serial) {;}
  Serial.println("Serial initialized.");

  screen = new ScreenUtil();
  
  gps = new GpsUtil();
}

/**
 * Main program loop
 */
void loop() 
{
  // read GPS and see if it's update time
  if (!gps->update())
    return;
  
  // refresh entire display
  screen->cls();

  // ------------------------------
  // -- refresh battery display
  // --
  String charge = battery.getCharge();
  
  // after analog read of battery pin, we must set it back to an input pin for the TFT display
  // https://forums.adafruit.com/viewtopic.php?f=57&t=139616&p=690795&hilit=TFT+analogRead#p690795 
  pinMode(VBATPIN, INPUT_PULLUP);
  
  screen->updateBatteryDisplay(charge);

  // ------------------------------
  // -- handle GPS location info if we have it
  // --
  if (gps->getGPS().fix) 
  {
    double alt = Pythagoras::cmToFeet(gps->getGPS().altitude);

    screen->println(0, 30, 2, HX8357_WHITE, "lat: " + String(gps->getGPS().latitudeDegrees, 10));
    screen->println(0, 60, 2, HX8357_WHITE, "lng: " + String(gps->getGPS().longitudeDegrees, 10));
    screen->println(0, 90, 2, HX8357_WHITE, "altitude (feet): " + String(alt));
    screen->println(0, 120, 2, HX8357_WHITE, "satellites: " + String(gps->getGPS().satellites));
  }
  else 
    screen->println(0, 30, 2, WHITE, "WHERE MY SATELLITES AT");
}

