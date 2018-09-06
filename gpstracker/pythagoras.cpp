#include "pythagoras.h"
#include <math.h>

#define METERS_TO_FEET 0.3048

float Pythagoras::cmToFeet(float cm)
{
  return cm * 0.032808;
}

float Pythagoras::degToRad(float deg)
{
  return deg * PI / 180.0;
}

float Pythagoras::scale(float x0, float x1, float y0, float y1, float x2)
{
	return fmax(y0, fmin(y1, (( x2 - x0 ) / ( x1 - x0 )) * ( y1 - y0 ) + y0));
}

position Pythagoras::stringToPosition(String str)
{
  int firstComma = str.indexOf(",");
  int secondComma = str.indexOf(",", firstComma+1);

  String lngStr = str.substring(0, firstComma);
  String latStr = str.substring(firstComma+1, secondComma);

//  Serial.println("#1S: " + latStr);
//  Serial.println("#2S: " + lngStr);

  position p;
  p.lat = latStr.toFloat();
  p.lng = lngStr.toFloat();

//  Serial.println("#1F: " + String(p.lat, GpsUtil::precision()));
//  Serial.println("#2F: " + String(p.lng, GpsUtil::precision()));
  
  return p;
}

/**
 * https://stackoverflow.com/questions/1502590/calculate-distance-between-two-points-in-google-maps-v3
 */
float Pythagoras::getFeetBetweenPositions(position p1, position p2)
{
  int R = 6378137; // Earth’s mean radius in meter
  float dLat = degToRad(p2.lat - p1.lat);
  float dLng = degToRad(p2.lng - p1.lng);
  float a = sin(dLat / 2) * sin(dLat / 2) + cos(degToRad(p1.lat)) * cos(degToRad(p2.lat)) * sin(dLng / 2) * sin(dLng / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float meters = R * c;
  float feet = meters * METERS_TO_FEET;
  
  return feet;
}

