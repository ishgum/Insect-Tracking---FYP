// display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//Display Modes
#define PRINT_EVERY_N  800  // PULSE mode always prints / updates every pulse
#define DIR_MAG             //display strongest dir & mag
#define RAW                 //display raw V values
//#define CRAPH               //display magnitude of differences with .'s (to make a graph of sorts)
#define DISP_MILLIS         // display rough time elasped since while loop beginning in millisec

unsigned long current_time, start_time = 0; //50 days before rollover
String output = "";


#endif

