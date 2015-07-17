// Sampling.h

#ifndef _SAMPLING_h
#define _SAMPLING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SamplingClass
{
 protected:


 public:
	void init();
	void getSample();

	// Variables
	float current_left, current_right;
	float average_left, average_right;


};

extern SamplingClass Sampling;

#endif

