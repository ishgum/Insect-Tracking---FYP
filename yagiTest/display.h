// display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "RunningAverage.h"
#include "Sampling.h"
//#include "yagiTest.ino"

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
#define BACKLED		   6

//Other
#define DIFFERENCE_THRESHOLD   0.1     // V, for max difference between Left and Right considered "the same" (0 to 5 valid)

extern SamplingClass Sampling; // Sampling is defined in Sampling.cpp // huhu woulda thunk it

// Function prototypes
void init_LEDs(void);
void displayData(float average_left, float average_right);
void serialResponse(void);
//void setLEDs(int led_config)


#endif
