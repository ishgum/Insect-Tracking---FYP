// Sampling.h

#ifndef _SAMPLING_h
#define _SAMPLING_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "RunningAverage.h"


#define ADC_TIMER_BUFFER_SIZE	5
#define ARDUINO_PWR_V			5
#define INTER_SAMPLE_BUFFER		7
/*  Setting details:
all are in Volts

ARDUINO_PWR_V		about 4.55V on USB, 5.0V with lipo

RIGHT_BIAS			testing found right is consistently higher than left, this is subtracted off right.
						used in all modes.

PULSE_THRESHOLD		the amount, (V), the RSSI amplitude has to be greater than the averaged
						amplitude to detect a pulse typ ~ 0.5V

LR_DIFF				max difference between Left and Right considered "the same"

MAX_DST				The smallest signal allowable before we want to move forward

MIN_DST				The strongest signal allowable before we want to move backwards

HYST			Hysteresis for switching between any two states.
						hysteresis is +/- HYSTERESIS, so total hysteresis band width
						is actually double HYSTERESIS
*/

// Ilam Fields test, on UAV. Generally expect stronger signals
#define RIGHT_BIAS			   0.12//0.05
#define PULSE_THRESHOLD        0.5
#define LR_DIFF				   0.07    
#define MAX_DST				   1.2
#define MIN_DST				   1.8
#define HYST				   0.01

//// Ilam Fields test, on ground
//#define RIGHT_BIAS				0.05	//0.12
//#define PULSE_THRESHOLD			0.1
//#define LR_DIFF					0.05    
//#define MAX_DST					1.2
//#define MIN_DST					1.8
//#define HYST					0.01

// CV Fields test
//#define RIGHT_BIAS			   0.15
//#define PULSE_THRESHOLD        0.6
//#define LR_DIFF				   0.15    
//#define MAX_DST				   1.5
//#define MIN_DST				   2.1
//#define HYST				   0.01

//// Indoors
//#define RIGHT_BIAS			   0.15
//#define PULSE_THRESHOLD        0.8
//#define LR_DIFF				   0.1
//#define MAX_DST				   1.8
//#define MIN_DST				   2.5
//#define HYST				   0.01


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
	RunningAverage pulse_left_buf;
	RunningAverage pulse_right_buf;
	RunningAverage buffer_left;
	RunningAverage buffer_right;
	float current_left, current_right; // sample from last reading
	float average_left, average_right; // averaged for continuous mode
	float noise_floor_left, noise_floor_right;		// estimate of noise floor for pulsed mode
	float pulse_left, pulse_right;					//result for pulsed mode.
	float pulse_left_av, pulse_right_av;
	Insect_state insect_state;
	uint8_t _buffer_size;
	bool _sampling_interrupt_buffer_full;
	uint8_t _idxProducer;
	uint8_t _idxConsumer;
	uint8_t _consumerDelay;
	float adc_isr_buffer[2][ADC_TIMER_BUFFER_SIZE];	// for adc ISR to fill, usage e.g [0][0] for 1st left, [1][4] for 5th right

private:
	bool _pulseOccuring;
	void interpretData(float average_left, float average_right);
	int _num_pulse_samples;
	int _left_pin, _right_pin;
	int _mode;// , _buffer_size;
	bool left_over_thresh, right_over_thresh;
//	float adc_isr_buffer[2][ADC_TIMER_BUFFER_SIZE];	// for adc ISR to fill, usage e.g [0][0] for 1st left, [1][4] for 5th right
	uint8_t _buffer_mutex;	// 1 if available to write
};

extern SamplingClass Sampling; // Sampling is declared in main (yagiTest)

#endif

