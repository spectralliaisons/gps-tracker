#include "pythagoras.h"
#include <math.h>

float Pythagoras::scale(float x0, float x1, float y0, float y1, float x2)
{
	return fmax(y0, fmin(y1, (( x2 - x0 ) / ( x1 - x0 )) * ( y1 - y0 ) + y0));
}

float Pythagoras::cmToFeet(float cm) {

  return cm * 0.032808;
}

