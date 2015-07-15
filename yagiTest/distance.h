// distance.h

#ifndef _DISTANCE_h
#define _DISTANCE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define STANDOFF_DISTANCE      20    //m, ideal insect distance 
#define LUT_SIZE               4      //number of entries in lookup table
const float distance_lut_middle[LUT_SIZE] = { 1.3, 1.9, 2.1, 2.3 };
const int distance_lut_out[LUT_SIZE] = { 50, 20, 10, 0 };

#endif

