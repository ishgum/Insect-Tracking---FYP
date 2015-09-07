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
void printBuffers(void) {
	//Timer1.stop();
	Serial.println("Display ISR buffer contents:");
	String buffer_output = "";
	for (int i = 0; i < 5; i++){
		buffer_output = "";
		buffer_output += i;
		buffer_output += "\tL:\t";
		buffer_output += Sampling.adc_isr_buffer[0][i];
		buffer_output += "\tR:\t";
		buffer_output += Sampling.adc_isr_buffer[1][i];
		Serial.println(buffer_output);
	}

	Serial.println("Display MAF buffer contents:");
	buffer_output = "";
	for (int i = 0; i < Sampling._buffer_size; i++){//SamplingClass::RunningAverage.getSize(); i++) {
		buffer_output = "";
		buffer_output += i;
		buffer_output += "\tL:\t";
		buffer_output += Sampling.getElement(i,0); // could use buffer_output += Sampling.buffer_left.getElement(i);
		buffer_output += "\tR:\t";
		buffer_output += Sampling.getElement(i, 1);
	//	buffer_output += "\tTest array:\t";
	//	buffer_output += _test[i];
		Serial.println(buffer_output);
	}
	//Timer1.resume();
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

	Serial.println(output);
	// code to limit output prints
	//else{
	//	N++;
	//	if (N > PRINT_EVERY_N) {
	//		N = 0;
	//		Serial.println(output);
	//	}
	Serial.println();
}

/*******************************************************************************
* Prints program internal state for debugging
*******************************************************************************/
void serialResponse(void){
	int incomingByte = Serial.read();    // required to clear serial receive buffer
	Serial.println("Serial Msg received");
	//printBuffers();
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
	static float left, right;
	static bool left_yagi = true; // what value is being sent through. true left, false right
	bool sample_finished = false;

	int incomingByte = Serial.read();    // required to clear serial receive buffer
	//Serial.println(incomingByte);
	// Leading character
	if (incomingByte == 10){ // == '\r'
		//Serial.println("think its tops");
		left_yagi = true;
		left = -1;
		right = -1;
	}
	else if (left_yagi){
		left = testDataMap(incomingByte);
		left_yagi = false;
	}
	else{
		right = testDataMap(incomingByte);
		sample_finished = true;
//		Serial.println("sending it off");
		Sampling.getTestSample(left, right);
		//printBuffers();
	}
	return sample_finished;
}

float testDataMap(int code){
	float result = -2.0;
	//Serial.print("was:  ");
	//Serial.println(code);
	if (code >= 15){
		// Assume integer, Convert to float
		// 0 to 5V in 0.1V increments
		//Serial.println("is ok");
		result = 0.01*(code-15)+0.4;
	}
	else{
		//Serial.println("that is not expected");
		result = -1.0;
	}
	//Serial.print("now:  ");
	//Serial.println(result);
	return result;
}