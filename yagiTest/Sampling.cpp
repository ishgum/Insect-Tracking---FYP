// Contains all code related to sampling the ADCs
// Class contains easily accessible buffer,
// RSSI magnitudes, pulse magnitudes, and rough time estimate
// Notes:
// Time rollover is 50 days

#include "Sampling.h"

SamplingClass::SamplingClass(int mode, int left_pin, int right_pin, int maf_size)
	:buffer_left(_buffer_size), buffer_right(_buffer_size){

	_mode = mode;
	_left_pin = left_pin;
	_right_pin = right_pin;
	_buffer_size = maf_size;

	// Fill buffer
	Serial.println("Filling buffer\n");
	while (buffer_left.getCount() < _buffer_size) { // wait until bufffer is full
		getSample();
	}
	Serial.println("Buffer full\n");
	average_left = buffer_left.getAverage();
	average_right = buffer_right.getAverage();
	noise_floor_left = average_left;
	noise_floor_right = average_right;
}

// Performs a sample without return
void SamplingClass::getSample(void){
	current_left = analogRead(_left_pin) * ARDUINO_PWR_V / 1023.0;
	current_right = analogRead(_right_pin) * ARDUINO_PWR_V / 1023.0;
}

void SamplingClass::continuousModeUpdate(void){
	getSample();
	buffer_left.addValue(current_left);
	buffer_right.addValue(current_right);
	average_left = buffer_left.getAverage();
	average_right = buffer_right.getAverage();
}

bool SamplingClass::pulseModeUpdate(void){
	getSample();
	// Find if reading exceeds noise floor
	left_over_thresh = current_left >= noise_floor_left + PULSE_THRESHOLD;
	right_over_thresh = current_right >= noise_floor_right + PULSE_THRESHOLD;

	// Pulse detected
	if ((left_over_thresh || right_over_thresh)) {
		delay(5);			// wait until pulse has risen (rough)
		getSample();
		pulse_left = current_left;
		pulse_right = current_right;
		delay(20);	// wait until pulse has stopped
		// signal pulse detected
		return true;
	}
	else{	// No pulse; update noise floor readings
		buffer_left.addValue(current_left);
		buffer_right.addValue(current_right);
		noise_floor_left = buffer_left.getAverage();
		noise_floor_right = buffer_right.getAverage();
	}
	return false;
}

/* 

*/
bool SamplingClass::fancyPulseModeUpdate(void){
	getSample();
	// Find if reading exceeds noise floor
	left_over_thresh = current_left >= noise_floor_left + PULSE_THRESHOLD;
	right_over_thresh = current_right >= noise_floor_right + PULSE_THRESHOLD;

	//fancy pulse
	if ((left_over_thresh || right_over_thresh)) {
		int num_pulse_samples = 0;
		delay(1); // ignore rising edge

		// get new sample which should be on peak of pulse
		getSample();
		left_over_thresh = current_left >= noise_floor_left + PULSE_THRESHOLD;
		right_over_thresh = current_right >= noise_floor_right + PULSE_THRESHOLD;
		if ((!left_over_thresh && !right_over_thresh)) {
			// pulse lasted 1 sample
			Serial.println("1 sample pulse, noise?");
		}

		while ((left_over_thresh || right_over_thresh)){ //during pulse
			pulse_left += current_left;
			pulse_right += current_right;
			num_pulse_samples++;
			getSample();
			left_over_thresh = current_left >= noise_floor_left + PULSE_THRESHOLD;
			right_over_thresh = current_right >= noise_floor_right + PULSE_THRESHOLD;
			
		}
		pulse_left = pulse_left / num_pulse_samples;
		pulse_right = pulse_right / num_pulse_samples;
		return true;
	}
	else{	// No pulse; update noise floor readings
		buffer_left.addValue(current_left);
		buffer_right.addValue(current_right);
		noise_floor_left = buffer_left.getAverage();
		noise_floor_right = buffer_right.getAverage();
	}
	return false;
}


float SamplingClass::getElement(int index, int dir){
	if (dir == 0){
		return buffer_left.getElement(index);
	}
	else if (dir == 1){
		return buffer_left.getElement(index);
	}
	else{
		error();
	}
}