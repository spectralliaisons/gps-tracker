
#include "gpsutil.h"
#include <Adafruit_GPS.h>
#include "sdutil.h"

#define GPSSerial Serial1
#define REFRESH_MS 2000

#define LOG_GPS true
#define LOG_NAME "GPSTEST.TXT" // "gpstrak.txt" // TODO: increment based on found files

#define LOG true

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

GpsUtil::GpsUtil()
{
  if (LOG_GPS)
  {
    // TODO: curr log name increments based on found files
    _currLog = LOG_NAME;
//    SDUtil::remove(_currLog);
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
	GPSSerial.println(PMTK_Q_RELEASE);

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

// read GPS and return true if new sentence received
bool GpsUtil::update()
{
	// ensure GPS is read
	char c = GPS.read();

	// if a sentence is received, we can check the checksum, parse it...
	if (GPS.newNMEAreceived())
	{
//  	Serial.println("GPS.newNMEAreceived YES");
//  	Serial.println(GPS.lastNMEA());
  
  	if (GPS.parse(GPS.lastNMEA()))
  	{
  		if (_timer->update())
  		{
        if (LOG)
        {
//          Serial.print("\nTime: ");
//          Serial.print(GPS.hour, DEC); Serial.print(':');
//          Serial.print(GPS.minute, DEC); Serial.print(':');
//          Serial.print(GPS.seconds, DEC); Serial.print('.');
//          Serial.println(GPS.milliseconds);
//          Serial.print("Date: ");
//          Serial.print(GPS.day, DEC); Serial.print('/');
//          Serial.print(GPS.month, DEC); Serial.print("/20");
//          Serial.println(GPS.year, DEC);
//          Serial.print("Fix: "); Serial.print((int)GPS.fix);
//          Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  
          if (GPS.fix)
          {
//            Serial.println();
//            Serial.println("------------------------ !!! YES WE RECEIVED A FIX !!! ------------------------");
//            Serial.print("FIX Location: ");
//            Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//            Serial.print(", ");
//            Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//            Serial.print("FIX Speed (knots): "); Serial.println(GPS.speed);
//            Serial.print("FIX Angle: "); Serial.println(GPS.angle);
//            Serial.print("FIX Altitude: "); Serial.println(GPS.altitude);
//            Serial.print("FIX Satellites: "); Serial.println((int)GPS.satellites);
//            Serial.println("-------------------------------------------------------------------------------");

//            logCurrentPosition();
          }
        }
        
  			return true;
  		}
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
  SDUtil::print(_currLog, posLn);
}
