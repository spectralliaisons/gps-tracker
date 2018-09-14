
#include "gpsutil.h"
#include <Adafruit_GPS.h>
#include "sdutil.h"

#define GPSSerial Serial1
#define REFRESH_MS 2000

#define LOG_GPS true
#define LOG_PREFIX "TRK"
#define DEBUG_LOG_NAME "GPSTEST.TXT"

#define LOG true

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

GpsUtil::GpsUtil()
{
  if (LOG_GPS)
  {
//    _currLog = SDUtil::increment(LOG_PREFIX);
//    SDUtil::remove(_currLog);
    _currLog = DEBUG_LOG_NAME;
  }

	// default NMEA GPS baud
	GPS.begin(9600);

	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

	// Set the update rate
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

	// Request updates on antenna status, comment out to keep quiet
	GPS.sendCommand(PGCMD_ANTENNA);

	// // Ask for firmware version
	// GPSSDUtil::log(PMTK_Q_RELEASE);

  _timer = new Timer("LOG_GPS", REFRESH_MS);
}

Adafruit_GPS GpsUtil::getGPS()
{
	return GPS;
}

/**
* Arduino floats only have 6 decimal precision, 
* but let's just say 10 becasue my physical gps saves kml files w/ 10 decimals.
*/
int GpsUtil::precision()
{
  return 10;
}

String GpsUtil::getFilepath()
{
  return _currLog;
}

bool GpsUtil::wasSentenceReceived()
{
  // ensure GPS is read
  char c = GPS.read();

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived())
    return GPS.parse(GPS.lastNMEA());
}

// read GPS and return true if new sentence received
bool GpsUtil::update()
{
	if (wasSentenceReceived())
  {
    if (_timer->update())
    {
      if (LOG)
      {
//          Serial.print("\nTime: ");
//          Serial.print(GPS.hour, DEC); Serial.print(':');
//          Serial.print(GPS.minute, DEC); Serial.print(':');
//          Serial.print(GPS.seconds, DEC); Serial.print('.');
//          SDUtil::log(GPS.milliseconds);
//          Serial.print("Date: ");
//          Serial.print(GPS.day, DEC); Serial.print('/');
//          Serial.print(GPS.month, DEC); Serial.print("/20");
//          SDUtil::log(GPS.year, DEC);
//          Serial.print("Fix: "); Serial.print((int)GPS.fix);
//          Serial.print(" quality: "); SDUtil::log((int)GPS.fixquality);

        if (GPS.fix)
        {
//            SDUtil::log();
//            SDUtil::log("------------------------ !!! YES WE RECEIVED A FIX !!! ------------------------");
//            Serial.print("FIX Location: ");
//            Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//            Serial.print(", ");
//            Serial.print(GPS.longitude, 4); SDUtil::log(GPS.lon);
//            Serial.print("FIX Speed (knots): "); SDUtil::log(GPS.speed);
//            Serial.print("FIX Angle: "); SDUtil::log(GPS.angle);
//            Serial.print("FIX Altitude: "); SDUtil::log(GPS.altitude);
//            Serial.print("FIX Satellites: "); SDUtil::log((int)GPS.satellites);
//            SDUtil::log("-------------------------------------------------------------------------------");

//            logCurrentPosition();
        }
      }
      
      return true;
    }
  }

	return false;
}

// log curr pos to SD card
void GpsUtil::logCurrentGeoloc()
{
  String latStr = String(GPS.latitudeDegrees, GpsUtil::precision());
  String lngStr = String(GPS.longitudeDegrees, GpsUtil::precision());
  String posLn = " " + lngStr + "," + latStr + ",0"; // TODO: what's up with the ",0"?
  SDUtil::write(_currLog, posLn, false, true);
}
