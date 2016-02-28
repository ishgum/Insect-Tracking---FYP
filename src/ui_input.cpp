/****************************************************************************
 *
 *
 * @file ui_input.cpp
 *
 * @brief Provides functions for inputs into the user interface
 *
 * Main function just reads the input and if the character matches one of the
 * commands it will change a flag so that the command gets executed
 * 
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------


#include "ui_input.h"



// ------------------------------------------------------------------------------
//   Flags
// ------------------------------------------------------------------------------


// Controls whether the program is paused or not (NB, not yet implemented!)
bool contPause;


// Controls when the camera is connected to (NB, no functionality for disconnecting from camera)
bool contCam;


// Gives the program a signal to quit. This is favoured over aborting as it will close the interfaces
// correctly
bool contQuit;


// Controls the connection to the UAV, this can be toggled on and off if offboard commands should stop
// being sent. (NB, the control of offboard in controlled by the remote, if the UAV is in offboard and
// then this control is toggled, the program should automatically revert to (?)
bool contUAV;


// Opens a small debug window
bool contDebugROI;


// Opens the full visible frame at a reduced size
bool contDebugFull;


// Refreshes all of the parts of the screen that aren't regularly updated
bool contRefresh;


bool contTest;


// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Input control
// ------------------------------------------------------------------------------

void inputControl(void) 
{
	// Receives a character from ncurses
	char currentInput = wgetch(output.outputStream);

	switch (currentInput) 
	{
		case 'p':
			if (contPause) { wprintw(output.outputStream, "Resuming program\n"); }
			else { wprintw(output.outputStream, "Pausing program\n"); }
			contPause = !contPause;
			break;
		
		
		case 'c':
			if (contCam) { wprintw(output.outputStream, "Camera already running\n"); }
			else { wprintw(output.outputStream, "Activating Camera\n"); }
			contCam = true;
			break;


		case 'q':
			wprintw(output.outputStream, "Program quitting\n");
			contQuit = true;
			break;


		case 'u':
			if (!contUAV) { wprintw(output.outputStream, "Enabling offboard control\n"); }
			else { wprintw(output.outputStream, "Disabling offboard control\n"); }
			contUAV = !contUAV;
			break;


		case 'd':
			if (!contDebugROI) { wprintw(output.outputStream, "Enabling ROI window. (Note: this will negatively affect performance)\n"); }
			else { wprintw(output.outputStream, "Closing ROI windows\n"); }
			contDebugROI = !contDebugROI;
			break;


		case 'f':
			if (!contDebugFull) { wprintw(output.outputStream, "Enabling Full frame window. (Note: this will negatively affect performance)\n"); }
			else { wprintw(output.outputStream, "Closing Full frame windows\n"); }
			contDebugFull = !contDebugFull;
			break;
			

		case 'r':
			contRefresh = true;
			wprintw(output.outputStream, "Refreshing screen\n");
			break;
			

		case 'e':
			wprintw(output.outputStream, "Echo\n");
			break;

		case 't':
			contTest = true;
			break;
			
	}
}



// ------------------------------------------------------------------------------
//   Output parameters to ncurses window
// ------------------------------------------------------------------------------

void printFlagParameters(int y, int x)
{
    map<const char*, int> outputMap;

	outputMap["Flag Info"] = 0;

	outputMap["ROI Debug:"] = 2;
	outputMap["Full Frame Debug:"] = 3;
	outputMap["Paused:"] = 4;
	outputMap["Running:"] = 5;

    printDataWindow(outputMap, y, x);

}


// ------------------------------------------------------------------------------
//   Refresh data in ncurses window
// ------------------------------------------------------------------------------

void printFlagOutput(void) {

	werase(output.flagData);
	wprintw(output.flagData, "\n\n");
	wprintw(output.flagData, "%d\n", contDebugROI);
	wprintw(output.flagData, "%d\n", contDebugFull);
	wprintw(output.flagData, "%d\n", contPause);	
	wprintw(output.flagData, "%d\n", !contQuit);
}

