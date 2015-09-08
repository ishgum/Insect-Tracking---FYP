/*******************************************************************************
* ENEL400 Insect Tracking FYP
* Dylan Mackie
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
enum Signal_mode {PULSE, SIMPLE_CONTINUOUS, PULSE_TEST, SERIAL_TEST}; // possible signal_modes

const Signal_mode MODE = PULSE;//SIMPLE_CONTINUOUS;				// Main mode switch for program
#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAF_SIZE               5    // 256 absolute max, 200 probably safe
#define ADC_SAMPLING_PERIOD	   2000	// us. 200 definitely too fast
// Pin dfns
#define LEFT_PIN       A0
#define RIGHT_PIN      A3

SamplingClass Sampling(MODE, LEFT_PIN, RIGHT_PIN, MAF_SIZE);

void setup() {
	pinMode(13, OUTPUT);
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

//	printBuffers();
	Sampling.fillBuffer();
	init_LEDs();

	

	// Select mode based on MODE
	if (MODE == PULSE){
		pulse();
	}
	else if (MODE == SIMPLE_CONTINUOUS){
		simple();
	}
	else if (MODE == SERIAL_TEST){
		serialTest();
	}
	else if (MODE == PULSE_TEST){
		pulseSerialData();
	}
	else{
		error();
	}
}

/*******************************************************************************
* timer ISR
* Sample ADC
*******************************************************************************/
void timerIsr(){
	Sampling.getSample();
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
* Simplest program mode
* Samples and prints to serial as fast as possible, typ 1 - 2ms sample period
*******************************************************************************/
void simple(void) {
  while(1){
    //Serial.println(millis());	// uncomment to print time too
    //Serial.print("\t");
    Serial.print((analogRead(LEFT_PIN))*ARDUINO_PWR_V/1023.0);
    Serial.print("\t");
    Serial.println((analogRead(RIGHT_PIN))*ARDUINO_PWR_V/1023.0);
	//Serial.print("\r\n");

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

	// init Timer interrupt for ADC sampling
	Timer1.initialize(ADC_SAMPLING_PERIOD); // e.g set a timer of length 1000 microseconds (or 0.001 sec - or 1kHz)
	Timer1.attachInterrupt(timerIsr); // attach the service routine here
	//Timer1.stop();
	Timer1.resume(); // start timed sampling
	while (1) {
		is_pulse = Sampling.pulseModeUpdate(); 		// Process sample buffer
		
		if (is_pulse){
			displayData(Sampling.pulse_left, Sampling.pulse_right);	// pulse detected, update display
			setLEDs(OFF);	// turn LEDs off again so we can see them flicker, relies on serial lag
		}

		// Check if we are too slow emptying adc buffer
		//Serial.print(Sampling._idxProducer);
		//Serial.print("\t");
		//Serial.println(Sampling._idxConsumer);
		//if (Sampling._sampling_interrupt_buffer_full){
		//	Serial.println("Slow");
		//	Sampling._sampling_interrupt_buffer_full = false; // crappy workaround for now
		//}

		// Check for incoming serial messages, and print status if we get anything
		// Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
		if (Serial.available() > 0) {	
			serialResponse();
		}
	}
}

/*******************************************************************************
* Pulse Mode Loopm using data fed from serial
As above
*******************************************************************************/
void pulseSerialData(){
	bool is_pulse = false;
	bool new_sample = false;
	//printBuffers();
	while (1) {
		//Serial.println("here");
		//delay(1000);
		is_pulse = Sampling.pulseModeUpdate(); 		// Process sample buffer

		if (is_pulse){
			//Serial.println("displaying");
			displayData(Sampling.pulse_left, Sampling.pulse_right);	// pulse detected, update display
			setLEDs(OFF);	// turn LEDs off again so we can see them flicker, relies on serial lag
		}

		// Check for incoming serial messages, and update buffer
		if (Serial.available() > 0) {
			//Serial.println("Got it");
			new_sample = serialTestData();
			//printBuffers();
		}
	}
}

/*******************************************************************************
* Loop needs to be defined somewhere for arduino main() to be happy
* might miss out on checking for serial events, see main() definition
*******************************************************************************/
void loop() {} 