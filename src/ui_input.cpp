
#include "ui_input.h"


bool contPause, contCam, contQuit, contUAV, contDebugROI, contDebugFull, contRefresh;

void inputControl(void) {

	char currentInput = wgetch(output.outputStream);
	switch (currentInput) {
		case 'p':
			if (contPause) wprintw(output.outputStream, "Resuming program\n");
			else wprintw(output.outputStream, "Pausing program\n");
			contPause = !contPause;
			break;
		
		
		case 'c':
			if (contCam) wprintw(output.outputStream, "Camera already running\n");
			else wprintw(output.outputStream, "Activating Camera\n");
			contCam = true;
			break;


		case 'q':
			wprintw(output.outputStream, "Program quitting\n");
			contQuit = true;
			break;

		case 'u':
			if (!contUAV) wprintw(output.outputStream, "Enabling offboard control\n");
			else wprintw(output.outputStream, "Disabling offboard control\n");
			contUAV = !contUAV;
			break;

		case 'd':
			if (!contDebugROI) wprintw(output.outputStream, "Enabling ROI window. (Note: this will negatively affector performance)\n");
			else wprintw(output.outputStream, "Closing ROI windows\n");
			contDebugROI = !contDebugROI;
			break;

		case 'f':
			if (!contDebugFull) wprintw(output.outputStream, "Enabling Full frame window. (Note: this will negatively affector performance)\n");
			else wprintw(output.outputStream, "Closing Full frame windows\n");
			contDebugFull = !contDebugFull;
			break;
			
		case 'r':
			contRefresh = true;
			wprintw(output.outputStream, "Refreshing screen\n");
			break;
			
		case 'e':
			wprintw(output.outputStream, "Echo\n");
			break;
			
	}
}


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


void printFlagOutput(void) {

	werase(output.flagData);
	wprintw(output.flagData, "\n\n");
	wprintw(output.flagData, "%d\n", contDebugROI);
	wprintw(output.flagData, "%d\n", contDebugFull);
	wprintw(output.flagData, "%d\n", contPause);	
	wprintw(output.flagData, "%d\n", !contQuit);
}

