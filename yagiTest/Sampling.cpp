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
	_idxConsumer = 0;
	_idxProducer = 0;
	_consumerDelay = 0;
	_buffer_mutex = 1;
	_pulseOccuring = false;
	_num_pulse_samples = 0;
}

/*******************************************************************************
* Fills RunningAverage buffer with sequential ADC samples
*******************************************************************************/
void SamplingClass::fillBuffer(void){
	// Fill buffer
	Serial.print("Filling Buffer\n");
	while (buffer_left.getCount() < _buffer_size) { // wait until bufffer is full
		// Manually fill buffer
		buffer_left.addValue(analogRead(_left_pin)*ARDUINO_PWR_V / 1023.0);
		buffer_right.addValue(analogRead(_right_pin)*ARDUINO_PWR_V / 1023.0);
		delay(2);
		//Serial.println(buffer_left.getCount());
		//continuousModeUpdate();
	}
	Serial.print("Buffer Full\n");
	average_left = buffer_left.getAverage();
	average_right = buffer_right.getAverage();
	noise_floor_left = average_left;
	noise_floor_right = average_right;
}

/*******************************************************************************
* Updates adc_isr_buffer with one ADC sample on both channels
*******************************************************************************/
void SamplingClass::getSample(void){
	adc_isr_buffer[0][_idxProducer] = analogRead(_left_pin)*ARDUINO_PWR_V / 1023.0;
	adc_isr_buffer[1][_idxProducer] = analogRead(_right_pin)*ARDUINO_PWR_V / 1023.0;
	_idxProducer++;
	if (_idxProducer >= 5){ // rollover
		_idxProducer = 0;
	}
	if (_idxProducer == _idxConsumer){
		// Consumer too slow, set flag here to print error later
	}
	_consumerDelay++;
}



/*******************************************************************************
* Pulse mode method
* Performs sample, and update for pulsed signals
* Averages all samples determined to sit on the pulse
*******************************************************************************/
bool SamplingClass::pulseModeUpdate(void){
	bool updatedState = false;
	if (_consumerDelay > 0){ // Something to do
		// Find if reading exceeds noise floor
		left_over_thresh = adc_isr_buffer[0][_idxConsumer] >= noise_floor_left + PULSE_THRESHOLD;
		right_over_thresh = adc_isr_buffer[1][_idxConsumer] >= noise_floor_right + PULSE_THRESHOLD;

		if ((left_over_thresh || right_over_thresh)) {
			if (_pulseOccuring) { // pulse was occuring on last update
				pulse_left += current_left;	//could check for overflow, but should only be incrementing 1-2
				pulse_right += current_right;
				_num_pulse_samples++;
			}
			else{
				_pulseOccuring = true;
				_num_pulse_samples = 1;
				pulse_left = current_left;
				pulse_right = current_right;
			}
		}
		else{ // Threshold not exceeded
			if (_pulseOccuring){ // pulse was occuring on last update
				if (_num_pulse_samples == 1){	// pulse lasted 1 sample
					Serial.println("1 sample pulse; noise?");
				}
				else{
					pulse_left = pulse_left / _num_pulse_samples;
					pulse_right = pulse_right / _num_pulse_samples;
					interpretData(pulse_left, pulse_right);	// Update bug position based on pulse
					_pulseOccuring = false;
					updatedState = true;
				}
			}
			else{ // No pulse occured last update
				buffer_left.addValue(current_left);
				buffer_right.addValue(current_right);
				noise_floor_left = buffer_left.getAverage();
				noise_floor_right = buffer_right.getAverage();
			}
		}
		_idxConsumer++;
		_consumerDelay--;
	}
	else
	{
		// Nothing to do, this task managed to run more than once in one sample period.
	}
	return updatedState;
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