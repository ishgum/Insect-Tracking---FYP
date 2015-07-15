// display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "RunningAverage.h"

//Display Modes
#define PRINT_EVERY_N  800  // PULSE mode always prints / updates every pulse
#define DIR_MAG             //display strongest dir & mag
#define RAW                 //display raw V values
//#define CRAPH               //display magnitude of differences with .'s (to make a graph of sorts)
#define DISP_MILLIS         // display rough time elasped since while loop beginning in millisec

// Pins
#define LEFTLED        9
#define RIGHTLED       7
#define MIDDLELED      8

//Other
#define DIFFERENCE_THRESHOLD   0.1     // V, for max difference between Left and Right considered "the same" (0 to 5 valid)

unsigned long current_time, start_time = 0; //50 days before rollover
String output = "";
String dir = "uninitialised";

// Function prototypes
void init_LEDs(void);
void display_data(float average_left, float average_right);
void serial_response(int _size, RunningAverage& _left_b, RunningAverage& _right_b, float *_test,
	float cur_left = -1, float cur_right = -1,
	float ave_left = -1, float ave_right = -1);


#endif

