/**
 * 2018.08.22 Wes Jackson
 * wes@wesmjackson.com
 * 
 * First, make sure we have a port for the board:
 * Tools > Port > Feather M0
 */

#include<stdlib.h>
#include <SPI.h>
#include "sdutil.h"
#include "battery.h"
#include "gpsutil.h"
#include "screenutil.h"

// battery pin for knowing battery life
// refer to: https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7
Battery *battery;

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
//  while (!Serial) {;} // only uncomment this if you're connected via USB or else board reset will not work!
  Serial.println("Serial initialized.");

  SDUtil::init();

  battery = new Battery(VBATPIN);

  screen = new ScreenUtil();
  
  gps = new GpsUtil();
}

/**
 * Main program loop
 */
void loop() 
{
  // e.g. debugging initialization
  if (!gps || !screen || !battery)
    return;
   
  // read GPS and see if it's update time
  if (!gps->update())
    return;

  // ------------------------------
  // -- handle GPS location info if we have it
  // --
  screen->updateGPSMap(gps->getFileFromDisc());
  screen->updateGPSText(gps->getGPS());

  // ------------------------------
  // -- refresh battery display
  // --
  String displayCharge = battery->displayCharge();
  
  // after analog read of battery pin, we must set it back to an input pin for the TFT display
  // https://forums.adafruit.com/viewtopic.php?f=57&t=139616&p=690795&hilit=TFT+analogRead#p690795 
  pinMode(VBATPIN, INPUT_PULLUP);
  
  screen->updateBatteryDisplay(displayCharge, battery->isLow());
}

