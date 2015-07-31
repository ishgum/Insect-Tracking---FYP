// General functions to display state and debug information via either:
//	Serial monitor
//	or LEDs

#include "display.h"
unsigned long current_time, start_time; //50 days before rollover


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

void setLEDs(Led_config led_config)
{
	switch (led_config){
	case OFF:
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, LOW);
		break;
	default:
		break;
	}
}
//print buffer contents for debugging
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



// determine output based on state of
// insect_dir in Sampling object.
// update LEDs and Serial
void displayData(float average_left, float average_right) {
	static int N = 0;
	String dir = "";
	float diff = average_left - average_right;
	float mag = abs(diff);
	switch (Sampling.insect_dir){
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
	output += current_time;
	output += "\t";
#endif
#ifdef DIR_MAG
	// output = "Strongest:\t";
	output += dir + "\t";
	output += mag;
#endif
#ifdef CRAPH
	output += "\t";
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

#if defined(PULSE_MODE)
	Serial.println(output);
#else
	N++;
	if (N > PRINT_EVERY_N) {
		N = 0;
		Serial.println(output);
	}
#endif
}

void serial_response(void){
	int incomingByte = Serial.read();    // required to clear serial receive buffer
	Serial.println("Serial Msg received");
	print_buffers();
	Serial.println("\nDisplay averages:");
	displayData(Sampling.average_left, Sampling.average_right);
	Serial.println("\nDisplay current:");
	displayData(Sampling.current_left, Sampling.current_right);
}