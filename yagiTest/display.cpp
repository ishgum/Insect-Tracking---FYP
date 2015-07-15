// General functions to display state and debug information via either:
//	Serial monitor
//	or LEDs

#include "display.h"

void init_LEDs(void){
	pinMode(LEFT_PIN, INPUT);
	pinMode(RIGHT_PIN, INPUT);
	pinMode(LEFTLED, OUTPUT);
	pinMode(RIGHTLED, OUTPUT);
	pinMode(MIDDLELED, OUTPUT);
	digitalWrite(LEFTLED, LOW);
	digitalWrite(RIGHTLED, LOW);
	digitalWrite(MIDDLELED, LOW);
}

//print buffer contents for debugging
void print_buffers(void) {
	Serial.print("Display buffer contents:\n");
	String buffer_output = "";
	for (int i = 0; i < MAFSIZE; i++) {
		buffer_output = "";
		buffer_output += i;
		buffer_output += "\tR:\t";
		buffer_output += left_b.getElement(i);
		buffer_output += "\tL:\t";
		buffer_output += right_b.getElement(i);
		buffer_output += "\tTest array:\t";
		buffer_output += test_array_l[i];
		Serial.println(buffer_output);
	}
}

void serial_response(float cur_left = -1, float cur_right = -1,
	float ave_left = -1, float ave_right = -1){
	int incomingByte = Serial.read();    // required to clear receive buffer
	print_buffers();
	Serial.println("\nSerial Msg received, Display averages:");
	display_data(ave_left, ave_right);
	Serial.print("\nDisplay current:\n");
	display_data(cur_left, cur_right);
}

void display_data(float average_left, float average_right) {
	float diff = average_left - average_right;
	float mag = abs(diff);
	if (diff > DIFFERENCE_THRESHOLD) {
		dir = "Left";
		digitalWrite(LEFTLED, HIGH);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, LOW);
	}
	else if (diff < -DIFFERENCE_THRESHOLD) {
		dir = "Right";
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, HIGH);
		digitalWrite(MIDDLELED, LOW);
	}
	else {
		dir = "Middle";
		digitalWrite(LEFTLED, LOW);
		digitalWrite(RIGHTLED, LOW);
		digitalWrite(MIDDLELED, HIGH);
	}

	//Create Serial output
	// Note that large serial msg's can a couple of ms at 115200
	// and 10's of ms at 9600 baud
	output = "";
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

