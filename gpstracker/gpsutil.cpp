#include "Arduino.h"
#include "gpsutil.h"

#define GPSSerial Serial1
#define REFRESH_MS 2000

#define LOG true

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

uint32_t timer = millis();

GpsUtil::GpsUtil()
{
	_refreshMs = REFRESH_MS;

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

  _start = millis();
}

Adafruit_GPS GpsUtil::getGPS()
{
	return GPS;
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
  		// if millis() or timer wraps around, we'll just reset it
  		if (timer > millis()) timer = millis();
  
  		// approximately every 2 seconds or so, print out the current stats
  		if (millis() - timer > _refreshMs)
  		{
  			timer = millis();

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
            Serial.println();
            Serial.println("------------------------ !!! YES WE RECEIVED A FIX !!! ------------------------");
            Serial.print("FIX Location: ");
            Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
            Serial.print(", ");
            Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
            Serial.print("FIX Speed (knots): "); Serial.println(GPS.speed);
            Serial.print("FIX Angle: "); Serial.println(GPS.angle);
            Serial.print("FIX Altitude: "); Serial.println(GPS.altitude);
            Serial.print("FIX Satellites: "); Serial.println((int)GPS.satellites);
            Serial.println("-------------------------------------------------------------------------------");
          }
          else
          {
            Serial.println(millis() - _start);
          } 
        }
        
  			return true;
  		}
  	}
	}

	return false;
}
