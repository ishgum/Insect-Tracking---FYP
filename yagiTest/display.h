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
#include "TimerOne.h"

/*******************************************************************************
* Various settings for display modes
*******************************************************************************/
const bool HAC_96 = true;				// true if using HAC_96 radio to transmit data, false if using local serial
													// disables LEDs, and performs neccessary serial msg changes.
#define PRINT_EVERY_N  100  // for CONTINUOUS mode, PULSE mode always prints every pulse
#define DIR             //display strongest dir & mag
//#define MAG             //display mag diff between yagis, abs(L-R)
#define RAW                 //display raw V values
//#define CRAPH               //display magnitude of differences with .'s (to make a graph of sorts)
#define DISP_MILLIS         // display rough time elasped since while loop beginning in millisec

// Pins
#define LEFTLED        9
#define RIGHTLED       7
#define MIDDLELED      8
#define BACKLED		   6

//Other
extern SamplingClass Sampling; // Sampling is declared in main

enum Led_config
{
	OFF
};

// Function prototypes
void init_LEDs(void);
void setLEDs(Led_config led_config);

void displayData(float average_left, float average_right);
void serialResponse(void);

bool serialTestData(void);
float testDataMap(int code);
void printBuffers(void);

void serialTest();	// main loop for testing serial comms to HAK_96 radio

#endif

