/* ENEL400 Insect Tracking FYP
Dylan Mackie, Michael Jones,
2015
OVERVIEW:
Just reading & comparing two ADC's
has provision to implement a Moving average filter,
just change MAF_SIZE #define to size of MA buffer.
Toggles 3 LED's to indicate which ADC signal is strongest.
Has various display modes to be (un)commented as needed.
These should be commented out for headless operation for max speed,
though at 115200baud this program should still be able to get a
sample at least every 5ms.

Has basic detect & compare pulses functionality
When no pulses are being detected the state can be queried by sending one character
to the serial port. Send a msg by selecting CR or NL in serial monitor window,
and hit enter in the msg window.

NOTES:
-Have to use 9600baud (bits per second) for HAC_96 radio

-Max ADC read rate is ~0.1ms
Can get ~ 1 sample per ms w/ DIR_MAG.
digitalWrite typ takes <20us so can do this every loop

-Tried adding 625 us delay in Serial.print (actually HardwareSerial.cpp in arduino installation folder)
to allow it to work with the HAC_96 radio. To be tested

TODO:
> debug serial response
> Move all Serial to suitable function for HAK
> Implement methods to comm with HAC_96 radio
> check performance at 9600 baud
> investigate new error when using MAF_SIZE = 1
> create pulse test code
> check time display
> Change output to have more states:
> clockwise, anticlockwise, forwards, back, stop
> fix  pulse averaging													done, check
>possibly trim rising & falling edges of pulse detect					done, check
to increase accuracy (if >3 samples achieved)
>Have some overall gain information
>scale comparison based on this gain information
>remove known pulse values from averaging buffer for noise floor		done, check
calculation
*/

#include "Sampling.h"
#include "display.h"
#include "distance.h"
#include "RunningAverage.h"

// Settings
enum Signal_mode { PULSE, CONTINUOUS, SIMPLE_CONTINUOUS, SERIAL_TEST}; // possible signal_modes

const Signal_mode MODE = PULSE;//SIMPLE_CONTINUOUS;				// Main mode switch for program
#define SIMPLE_PULSE   //defined: uses basic check against MAF, then delays 5ms and samples to determine pulse value,
                          //otherwise use a more complicated mode that averages all samples during the pulse.
#define TEST_MODE                      //Use test array not ADC readings
#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAF_SIZE               5    // 256 absolute max, 200 probably safe

// Pin dfns
#define LEFT_PIN       A0
#define RIGHT_PIN      A3
//unsigned long current_time, start_time; //50 days before rollover
SamplingClass Sampling(MODE, LEFT_PIN, RIGHT_PIN, MAF_SIZE);

//enum Insect_dir
//{
//	CENTERED,
//	LEFT,
//	RIGHT,
//	TOO_FAR,
//	TOO_CLOSE
//};
//Insect_dir insect_dir = Insect_dir::CENTERED;

float test_array_l[MAF_SIZE];
float test_array_r[MAF_SIZE];

void setup() {
	if (HAC_96){
		Serial.begin(9600);    // radio requirements
		delay(500);
		Serial.print(1);
		delay(2);
		Serial.println();
	}
	else
	{
		Serial.begin(115200);    //for speed!
	}

	pinMode(LEFT_PIN, INPUT);
	pinMode(RIGHT_PIN, INPUT);
	delay(500);

	Sampling.fillBuffer();
	init_LEDs();

	// fill test array
	init_test_arrays();

	// Select mode based on MODE
	if (MODE == PULSE){
		pulse();
	}
	else if (MODE == CONTINUOUS){
		continuous();
	}
	else if (MODE == SIMPLE_CONTINUOUS){
		simple();
	}
	else if (MODE == SERIAL_TEST){
		serialTest();
	}
	else{
		error();
	}
}

// super simple error call
void error(void){
	Serial.println("ERROR");
	while (1){
		delay(1000);
		Serial.println("ERROR");
	}
}

// Used to debug pulse detection
void init_test_arrays(void) {
  for (int i = 0; i < MAF_SIZE - 5; i++) {
    test_array_l[i] = 0.0;
  }
  test_array_l[MAF_SIZE - 5] = 0.3;
  test_array_l[MAF_SIZE - 4] = 1.0;
  test_array_l[MAF_SIZE - 3] = 1.0;
  test_array_l[MAF_SIZE - 2] = 1.0;
  test_array_l[MAF_SIZE - 1] = 0.3;
}

// simple method to return next test_array sample
float get_test_sample(float * _sample_array, int _size){
  // Init index
  static int _idx = 0;
  //increment index
  _idx++;
  // check for index exceeding bounds
  if (_idx == _size){
    _idx = 0;
  }
  return _sample_array[_idx];
}

// Used to debug pulse detection
float get_random_sample(void){
  // return value between 0.5V an 2.5V
  return random(0,20)/10.0+0.5;
}

// Super simple mode, prints as fast as possible, typ 1 - 2ms
void simple(void) {
  while(1){
    //Serial.print(millis()-start_time);	// uncomment to print time too
    //Serial.print("\t");
    Serial.print((analogRead(LEFT_PIN))*ARDUINO_PWR_V/1023.0);
    Serial.print("\t");
    Serial.println((analogRead(RIGHT_PIN))*ARDUINO_PWR_V/1023.0);
  }
}

/*  Continuous Mode loop:
    Sample two channels as fast as possible (+ 100us delay),
    Add each sample to one of two RollingAverage buffers,
    and find the average.
    If the average for one buffer is greater than the other (within some tolerance DIFFERENCE_THRESHOLD),
    Then that channel is said to have a greater RSSI, which is indicated by the LED's,
    and Serial output if there are suitable display defines.
    LED's updated every sample.
    Serial updated every PRINT_EVERY_N sample to slow down display.
    */
void continuous(void) {

  while (1) {
    //Sample
	Sampling.continuousModeUpdate();


	displayData(Sampling.average_left, Sampling.average_right);
    delayMicroseconds(100); //max ADC speed given as 100us
    
    // Check for incoming serial messages, and print status if we get anything
    // Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
    if (Serial.available() > 0) {
		serialResponse();
    }
  }
}

/*  Pulse Mode Loop:
    Sample two channels as fast as possible (+ 100us delay),
    Add each sample to one of two RollingAverage buffers,
    and find the average.
    The noise floor is defined as these averages.
    If the current reading for one sample is greater than noise floor + PULSE_THRESHOLD,
    Then a pulse is said to be occuring.


    The average amplitude for each channel during the pulse is then compared (with DIFFERENCE_THRESHOLD again),
    if one is greater then,
    that channel is said to have a greater RSSI, which is indicated by the LED's,
    and Serial output if there are suitable display defines.
    LED's updated every pulse.
    Serial updated every pulse.
    */
void pulse(){
	bool is_pulse = false;


	int test_indx = 0;  // for debugging using a fixed test array

	unsigned long start_time = millis();

	while (1) {
		//Sample
		is_pulse = Sampling.pulseModeUpdate();
		// use test array
		//    current_left = test_array_l[test_indx];
		//    current_right = test_array_r[test_indx++];
		//    if (test_indx >MAF_SIZE){
		//      test_indx = 0;
		//    }
		// end debug test
		if (is_pulse){
			displayData(Sampling.pulse_left, Sampling.pulse_right);
			setLEDs(OFF);	// turn LEDs off so we can see them flicker with pulse
		}

		delayMicroseconds(100); //max ADC speed given as 100us
		// Check for incoming serial messages, and print status if we get anything
		// Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
		if (Serial.available() > 0) {	
			serialResponse();
		}
	}
}

void loop() {} // needs to be defined somewhere for arduino main() file to be happy
				// might miss out on checking for serial events, see main() definition

