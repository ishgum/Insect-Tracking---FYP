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
	insect_dir = Insect_dir::CENTERED;

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

/*
Determines RSSI for pulsed signals
Returns value sampled 5ms after pulse detected.
Sampled value should sit 5ms to 10ms into pulse
*/
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
Determines RSSI for pulsed signals
Averages the pulse.
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

// returns an element of the buffer, useful for debug
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

// Logic for interpreting the left and right yagi RSSI
void SamplingClass::interpretData(float average_left, float average_right){
	float diff = average_left - average_right;
	float mag = abs(diff);
	bool too_weak = (average_left < MAX_DST && average_right < MAX_DST);
	bool too_strong = (average_left > MIN_DST || average_right > MIN_DST);

	if (diff>DIFFERENCE_THRESHOLD){
		insect_dir = LEFT;
	}
	else if (diff < -DIFFERENCE_THRESHOLD){
		insect_dir = RIGHT;
	}
	else if (too_weak){
		insect_dir = TOO_FAR;
	}
	else if (too_strong){
		insect_dir = TOO_CLOSE;
	}
	else {
		insect_dir = CENTERED;
	}
}