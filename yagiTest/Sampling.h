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
#define PULSE_THRESHOLD        0.5     // V, the amount the RSSI amplitude has to be greater than the averaged
										// amplitude to detect a pulse (0 to 5 valid)
#define DIFFERENCE_THRESHOLD   0.1     // V, for max difference between Left and Right considered "the same" (0 to 5 valid)
#define MAX_DST				   1.25
#define MIN_DST				   1.9

enum Insect_dir
{
	CENTERED,
	LEFT,
	RIGHT,
	TOO_FAR,
	TOO_CLOSE
};

class SamplingClass
{
protected:
public:
	// Methods
	SamplingClass(int mode, int left_pin, int right_pin, int maf_size);
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
	Insect_dir insect_dir;

	int _buffer_size;

private:
	void interpretData(float average_left, float average_right);
	void getSample();

	int _left_pin, _right_pin;
	int _mode;// , _buffer_size;
	bool left_over_thresh, right_over_thresh;
};

extern SamplingClass Sampling; // Sampling is defined in main (yagiTest)

#endif

