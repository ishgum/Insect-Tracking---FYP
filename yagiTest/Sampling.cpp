/*******************************************************************************
* FILE: Sampling.cpp
* Contains all code related to sampling the ADCs
* Class contains easily accessible buffer,
* RSSI magnitudes, pulse magnitudes, and rough time estimate
* NOTES:
* Time rollover is 50 days
*******************************************************************************/
#include "Sampling.h"

/*******************************************************************************
* SamplingClass Constructor
*******************************************************************************/
SamplingClass::SamplingClass(int mode, int left_pin, int right_pin, uint8_t maf_size)
	:buffer_left(maf_size), buffer_right(maf_size){

	_mode = mode;
	_left_pin = left_pin;
	_right_pin = right_pin;
	_buffer_size = maf_size;
	insect_state = CENTERED;
}

/*******************************************************************************
* Fills RunningAverage buffer with sequential ADC samples
*******************************************************************************/
void SamplingClass::fillBuffer(void){
	// Fill buffer
	Serial.print("Filling Buffer\n");
	while (buffer_left.getCount() < _buffer_size) { // wait until bufffer is full
		//Serial.println(buffer_left.getCount());
		continuousModeUpdate();
	}
	Serial.print("Buffer Full\n");
	average_left = buffer_left.getAverage();
	average_right = buffer_right.getAverage();
	noise_floor_left = average_left;
	noise_floor_right = average_right;
}

/*******************************************************************************
* Updates current reading with one ADC sample on both channels
*******************************************************************************/
void SamplingClass::getSample(void){
	current_left = analogRead(_left_pin) * ARDUINO_PWR_V / 1023.0;
	current_right = analogRead(_right_pin) * ARDUINO_PWR_V / 1023.0;
}

/*******************************************************************************
* Continuous mode method
* Performs sample, buffer update, and updates average reading
* Averaged reading is then interpreted to update insect state
*******************************************************************************/
void SamplingClass::continuousModeUpdate(void){
	getSample();								// Sample ADC
	buffer_left.addValue(current_left);			// Add values to buffers
	buffer_right.addValue(current_right);
	average_left = buffer_left.getAverage();	// Find new average value
	average_right = buffer_right.getAverage();
	interpretData(average_left, average_right);	// Update bug position based on average
}

/*******************************************************************************
* Simplified pulse mode method
* Depreciated once the more complex mode is tested working
* Performs sample, and update for pulsed signals
* Uses value sampled 5ms after pulse detected to determine pulse amplitude
* Sampled value should sit 5ms to 10ms into pulse
*******************************************************************************/
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
		interpretData(pulse_left, pulse_right);	// Update bug position based on pulse
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

/*******************************************************************************
* Pulse mode method
* Performs sample, and update for pulsed signals
* Averages all samples determined to sit on the pulse
*******************************************************************************/
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
		interpretData(pulse_left, pulse_right);	// Update bug position based on pulse
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

/*******************************************************************************
* Returns an element of the buffers
* dir 0 = left, 1 = right
*******************************************************************************/
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

/*******************************************************************************
* Updates insect state based on passed RSSI readings
*******************************************************************************/
void SamplingClass::interpretData(float average_left, float average_right){
	float diff = average_left - average_right;
	float mag = abs(diff);
	bool too_weak = (average_left < MAX_DST && average_right < MAX_DST);	// if both Yagi RSSI's are too weak
	bool too_strong = (average_left > MIN_DST || average_right > MIN_DST);	// if at least one Yagi RSSI is too strong

	if (diff>DIFFERENCE_THRESHOLD){
		insect_state = LEFT;
	}
	else if (diff < -DIFFERENCE_THRESHOLD){
		insect_state = RIGHT;
	}
	else if (too_weak){
		insect_state = TOO_FAR;
	}
	else if (too_strong){
		insect_state = TOO_CLOSE;
	}
	else {
		insect_state = CENTERED;
	}
}