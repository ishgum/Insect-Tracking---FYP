// Sampling.h

#ifndef _SAMPLING_h
#define _SAMPLING_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "RunningAverage.h"


#define ADC_TIMER_BUFFER_SIZE  10
#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define PULSE_THRESHOLD        0.5     // V, the amount the RSSI amplitude has to be greater than the averaged
										// amplitude to detect a pulse (0 to 5 valid) typ ~ 0.5V
#define DIFFERENCE_THRESHOLD   0.25     // V, for max difference between Left and Right considered "the same" (0 to 5 valid)
#define MAX_DST				   1.25
#define MIN_DST				   1.9
#define HYSTERESIS			   0.0		// hysteresis for switching between any two states.
											// hysteresis is +/- HYSTERESIS, so total hysteresis band width
												// is actually double HYSTERESIS
void error(void);
extern void myPrintString(const char input_str[]);
void delayInISR(int micro_seconds);

enum Insect_state
{
	CENTERED,
	LEFT,
	RIGHT,
	TOO_FAR,
	TOO_CLOSE
};

/*******************************************************************************
* Class to implement all ADC sampling and signal interpretation functions
*******************************************************************************/
class SamplingClass
{
protected:
public:
	// Methods
	SamplingClass(int mode, int left_pin, int right_pin, uint8_t maf_size);
	void fillBuffer(void);

	bool pulseModeUpdate(void);
	void getSample();
	void getTestSample(float left, float right); 
	float getElement(int index, int dir);

	// Variables
	RunningAverage buffer_left;
	RunningAverage buffer_right;
	float current_left, current_right; // sample from last reading
	float average_left, average_right; // averaged for continuous mode
	float noise_floor_left, noise_floor_right;		// estimate of noise floor for pulsed mode
	float pulse_left, pulse_right;					//result for pulsed mode.
	Insect_state insect_state;
	uint8_t _buffer_size;
	bool _sampling_interrupt_buffer_full;
	uint8_t _idxProducer;
	uint8_t _idxConsumer;
	uint8_t _consumerDelay;

private:
	bool _pulseOccuring;
	void interpretData(float average_left, float average_right);
	int _num_pulse_samples;
	int _left_pin, _right_pin;
	int _mode;// , _buffer_size;
	bool left_over_thresh, right_over_thresh;
	float adc_isr_buffer[2][ADC_TIMER_BUFFER_SIZE];	// for adc ISR to fill, usage e.g [0][0] for 1st left, [1][4] for 5th right
	uint8_t _buffer_mutex;	// 1 if available to write
};

extern SamplingClass Sampling; // Sampling is declared in main (yagiTest)

#endif
