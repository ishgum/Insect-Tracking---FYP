/*******************************************************************************
* FILE: display.cpp
* General functions to display state and debug information via Serial and LEDs
*******************************************************************************/

#include "display.h"
unsigned long current_time, start_time; //50 days before rollover

/*******************************************************************************
* Sets LED pin modes and inital state to off
*******************************************************************************/
void init_LEDs(void){
	pinMode(LEFTLED, OUTPUT);
	pinMode(RIGHTLED, OUTPUT);
	pinMode(MIDDLELED, OUTPUT);
	pinMode(BACKLED, OUTPUT);
	digitalWrite(LEFTLED, LOW);
	digitalWrite(RIGHTLED, LOW);
	digitalWrite(MIDDLELED, LOW);
	digitalWrite(BACKLED, LOW);
}

/*******************************************************************************
* Set all LEDs to OFF
*******************************************************************************/
void setLEDs(Led_config led_config)
{
	switch (led_config){
	case OFF:
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, LOW);
		digitalWrite(BACKLED, LOW);
		break;
	default:
		break;
	}
}

/*******************************************************************************
* Prints both buffers contents for debugging
*******************************************************************************/
void print_buffers(void) {
	Serial.print("Display buffer contents:\n");
	String buffer_output = "";
	for (int i = 0; i < Sampling._buffer_size; i++){//SamplingClass::RunningAverage.getSize(); i++) {
		buffer_output = "";
		buffer_output += i;
		buffer_output += "\tR:\t";
		buffer_output += Sampling.getElement(i,0); // could use buffer_output += Sampling.buffer_left.getElement(i);
		buffer_output += "\tL:\t";
		buffer_output += Sampling.getElement(i, 1);
	//	buffer_output += "\tTest array:\t";
	//	buffer_output += _test[i];
		Serial.println(buffer_output);
	}
}


/*******************************************************************************
* Prints insect state and other optional information to Serial.
* Also updates LED states
* Options toggled by #defines in display.h
*******************************************************************************/
void displayData(float average_left, float average_right) {
	static int N = 0;
	String dir = "";	// String uses dynamic memory allocation, possibly not ideal.
	float diff = average_left - average_right;
	float mag = abs(diff);
	switch (Sampling.insect_state){
	case LEFT:
		dir = "Left";
		digitalWrite(LEFTLED, HIGH);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, LOW);
		digitalWrite(BACKLED, LOW);
		break;
	case RIGHT:
		dir = "Right";
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, HIGH);
		digitalWrite(MIDDLELED, LOW);
		digitalWrite(BACKLED, LOW);
		break;
	case TOO_FAR:
		dir = "Forwards";
		digitalWrite(LEFTLED, HIGH);
		digitalWrite(RIGHTLED, HIGH);
		digitalWrite(MIDDLELED, HIGH);
		digitalWrite(BACKLED, LOW);
		break;
	case TOO_CLOSE:
		dir = "Back";
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, LOW);
		digitalWrite(BACKLED, HIGH);
		break;
	case CENTERED:
		dir = "Middle";
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, HIGH);
		digitalWrite(BACKLED, LOW);
		break;
	default:
		error();
		break;
	}

	//Create Serial output
	// Note that large serial msg's can take a couple of ms at 115200
	// and 10's of ms at 9600 baud
	String output = "";
#ifdef DISP_MILLIS
	current_time = millis() - start_time;
	float current_time_s = current_time / 1000.0;
	output += current_time_s;
	output += "\t";
#endif
#ifdef DIR
	// output = "Strongest:\t";
	output += dir + "\t";
#endif
#ifdef MAG
	output += mag;
	output += "\t";
#endif // MAG

#ifdef CRAPH
	for (int i = 0; i < mag * 10 && i < 100; i++) {
		output += ".";
		if (i == 99) {
			output += "+!";
		}
	}
#endif
#ifdef RAW
	output += "\tRAW: L:\t";
	output += String(average_left);
	output += "\tR:\t";
	output += String(average_right);
#endif

	Serial.println(output);
	// code to limit output prints
	//else{
	//	N++;
	//	if (N > PRINT_EVERY_N) {
	//		N = 0;
	//		Serial.println(output);
	//	}
	
}

/*******************************************************************************
* Prints program internal state for debugging
*******************************************************************************/
void serialResponse(void){
	int incomingByte = Serial.read();    // required to clear serial receive buffer
	Serial.println("Serial Msg received");
	//print_buffers();
	/*Serial.println("\nDisplay averages:");
	displayData(Sampling.average_left, Sampling.average_right);
	Serial.println("\nDisplay current:");
	displayData(Sampling.current_left, Sampling.current_right);
	*/
	Serial.println("\nDisplay pulses:");
	Serial.println(Sampling.pulse_left);
	Serial.println(Sampling.pulse_right);
	//displayData(Sampling.pulse_left, Sampling.pulse_right);
	Serial.println();
}

/*******************************************************************************
* Hello World used to debug transmit only comms on HAK_96 radio
*******************************************************************************/
void serialTest(){
	uint8_t counter = 0;
	while (1){
		Serial.print("Hello World\t");
		Serial.print(counter);
		Serial.print("\r\n");
		counter++;
		delay(1000);
	}
}

/*******************************************************************************
* For serial data test mode
*******************************************************************************/
bool serialTestData(void){
	float left, right;
	static bool left_yagi = true; // what value is being sent through. true left, false right
	bool sample_finished = false;

	int incomingByte = Serial.read();    // required to clear serial receive buffer
	
	// Leading character
	if (incomingByte == 255){
		left_yagi = true;
	}
	else if (left_yagi){
		left = testDataMap(incomingByte);
		left_yagi = false;
	}
	else{
		right = testDataMap(incomingByte);
		sample_finished = true;
		Sampling.getTestSample(left, right);
	}
	return sample_finished;
}

float testDataMap(int code){
	// Assume integer, Convert to float
	// 0 to 5V in 0.1V increments
	float result = 0.1*code;
	return result;
}