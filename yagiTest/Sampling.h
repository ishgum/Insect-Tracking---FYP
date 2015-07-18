// Sampling.h

#ifndef _SAMPLING_h
#define _SAMPLING_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "RunningAverage.h"
void error(void);

#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAF_SIZE                50    // 256 absolute max, 200 probably safe
#define PULSE_THRESHOLD        0.5     // V, the amount the RSSI amplitude has to be greater than the averaged
// amplitude to detect a pulse (0 to 5 valid)

// Pin dfns
//#define LEFT_PIN       A0
//#define RIGHT_PIN      A3



class SamplingClass
{
protected:
public:
	// Methods
	SamplingClass(int mode);
	void getSample();
	void continuousModeUpdate(void);
	bool pulseModeUpdate(void);
	bool fancyPulseModeUpdate(void);

	float getElement(int index, int dir);

	// Variables
	RunningAverage buffer_left;
	RunningAverage buffer_right;
	float current_left, current_right; // sample from last reading
	float average_left, average_right; // averaged for continuous mode
	float noise_floor_left, noise_floor_right;		// estimate of noise floor for pulsed mode
	float pulse_left, pulse_right;					//result for pulsed mode.
	
	//Consts
	const int _buffer_size = MAF_SIZE;

private:
	int _mode;
	bool left_over_thresh, right_over_thresh;
	//usigned long start
};

extern SamplingClass Sampling; // somewhere Sampling is defined
//SamplingClass Sampling;			// Create instance of sampling object

#endif

