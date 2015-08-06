/*******************************************************************************
* ENEL400 Insect Tracking FYP
* Dylan Mackie, Michael Jones
* 2015
*******************************************************************************
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
*******************************************************************************
NOTES:

* Have to use 9600baud (bits per second) for HAC_96 radio
* See yagiReadme, also had to add 2ms delay between bytes for radio

-Max ADC read rate is ~0.1ms
Can get ~ 1 sample per ms w/ DIR_MAG.
digitalWrite typ takes <20us so can do this every loop

*******************************************************************************
TODO:
> debug purple LED staying on
> debug serial response
> Move all Serial to suitable function for HAK
	> limit serial comms due to slow serial
> Implement methods to comm with HAC_96 radio
> check performance at 9600 baud
> investigate new error when using MAF_SIZE = 1
> create pulse test code
> check time display
> Change output to have more states:
> clockwise, anticlockwise, forwards, back, stop						done, check
> fix  pulse averaging													done, check
>possibly trim rising & falling edges of pulse detect					done, check
to increase accuracy (if >3 samples achieved)
>Have some overall gain information
>scale comparison based on this gain information if helpful
>remove known pulse values from averaging buffer for noise floor		done, check
calculation
> consider using timer interrupts for ADC sampling to mitigate serial comms delay
*******************************************************************************/

#include "TimerOne.h"
#include "Sampling.h"
#include "display.h"
#include "distance.h"
#include "RunningAverage.h"

// Settings
enum Signal_mode { PULSE, CONTINUOUS, SIMPLE_CONTINUOUS, SERIAL_TEST}; // possible signal_modes

const Signal_mode MODE = CONTINUOUS;//SIMPLE_CONTINUOUS;				// Main mode switch for program
#define SIMPLE_PULSE   //defined: uses basic check against MAF, then delays 5ms and samples to determine pulse value,
                          //otherwise use a more complicated mode that averages all samples during the pulse.
#define TEST_MODE                      //Use test array not ADC readings
#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAF_SIZE               5    // 256 absolute max, 200 probably safe

// Pin dfns
#define LEFT_PIN       A0
#define RIGHT_PIN      A3

SamplingClass Sampling(MODE, LEFT_PIN, RIGHT_PIN, MAF_SIZE);

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

	// init Timer interrupt for ADC sampling
	Timer1.initialize(2000); // set a timer of length 1000 microseconds (or 0.001 sec - or 1kHz)
	Timer1.attachInterrupt(timerIsr); // attach the service routine here

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

/*******************************************************************************
* timer ISR
* Sample ADC and perform a lot of processing despite best practices b/c how
* should I buffer it otherwise?
*******************************************************************************/
void timerIsr(){

}


/*******************************************************************************
* Super basic error loop
*******************************************************************************/
void error(void){
	while (1){
		Serial.println("ERROR");
		delay(1000);
	}
}

/*******************************************************************************
* Init test arrays used to debug pulse detection
*******************************************************************************/
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

/*******************************************************************************
* Return next test array sample
*******************************************************************************/
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

/*******************************************************************************
* Return random test sample to debug pulse detection
*******************************************************************************/
float get_random_sample(void){
  // return value between 0.5V an 2.5V
  return random(0,20)/10.0+0.5;
}

/*******************************************************************************
* Simplest program mode
* Samples and prints to serial as fast as possible, typ 1 - 2ms sample period
*******************************************************************************/
void simple(void) {
  while(1){
    //Serial.print(millis()-start_time);	// uncomment to print time too
    //Serial.print("\t");
    Serial.print((analogRead(LEFT_PIN))*ARDUINO_PWR_V/1023.0);
    Serial.print("\t");
    Serial.println((analogRead(RIGHT_PIN))*ARDUINO_PWR_V/1023.0);
	//Serial.print("\r\n");

  }
}

/*******************************************************************************
* Continuous program mode
* Sample two channels as fast as possible,
* Add each sample to one of two RollingAverage buffers,
* and find the average.
* If the average for one buffer is greater than the other (within some tolerance DIFFERENCE_THRESHOLD),
* Then that channel is said to have a greater RSSI, which is indicated by the LED's,
* and Serial output if there are suitable display defines.
* LED's updated every sample.
* Serial updated every PRINT_EVERY_N sample to slow down display.
*******************************************************************************/
void continuous(void) {

  while (1) {
    //Sample
	Sampling.continuousModeUpdate();
	displayData(Sampling.average_left, Sampling.average_right);
    delayMicroseconds(100); //max ADC speed given as 100us, serial takes much longer than this anyway
    // Check for incoming serial messages, and print status if we get anything
    // Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
    if (Serial.available() > 0) {
		serialResponse();
    }
  }
}

/*******************************************************************************
* Pulse Mode Loop:
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
*******************************************************************************/
void pulse(){
	bool is_pulse = false;
	int test_indx = 0;  // for debugging using a fixed test array
	unsigned long start_time = millis();

	while (1) {
		//Sample
		is_pulse = Sampling.pulseModeUpdate();
		
		// use test array for debug
		//current_left = get_test_sample(test_array_l, MAF_SIZE);
		//current_right = get_test_sample(test_array_r, MAF_SIZE);

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

/*******************************************************************************
* Loop needs to be defined somewhere for arduino main() to be happy
* might miss out on checking for serial events, see main() definition
*******************************************************************************/
void loop() {} 
