/****************************************************************************
 *
 *
 * @file main.cpp
 *
 * @brief Brings together all of the submodules and manages the user interface
 *
 * Runs the main program which executes all of the sub modules
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>


#include <fstream>
#include <iomanip>

#include <time.h>
#include <signal.h>

#include <ncurses.h>

#include "include/insect.h"
#include "include/camera_interface.h"
#include "include/common.h"
#include "include/mavlink_interface.h"
#include "include/ui_input.h"


// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace cv;
using namespace std;


// ------------------------------------------------------------------------------
//   Defines
// ------------------------------------------------------------------------------

// Note that opencv requires that there be a least 1 wait key function per loop in
// order to display images. The number is the delay in msec.

#define WAIT_PERIOD	5
#define BILLION  1E9

ofstream logFile;
bool writeFile;
// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------


void createLog(void) 
{
    time_t ltime = time(NULL); /* calendar time */
    writeFile = false;
    char buffer[40] = {0};
    strftime(buffer, sizeof(buffer), "../logs/log-%d-%b-%y-%T.txt", localtime(&ltime));

    logFile.open(buffer);
    if (logFile.is_open()) 
    { 
	    writeFile = true; 
	    logFile << "Log for insect tracking test flight" << endl;
	    logFile << setw(15) <<  left << "Local time";
        logFile << setw(15) << "North output";
        logFile << setw(15) << "East output";
        logFile << setw(15) << "Insect Found";
        logFile << setw(15) << "Battery Status";
        logFile << setw(15) << "UAV Pos X";
        logFile << setw(15) << "UAV Pos Y";
        logFile << setw(15) << "UAV Yaw";
        logFile << endl;
    }
    else { wprintw(output.outputStream, "File not able to be written - no log file\n"); }
}



void updateLog(Insect* insect, UAVControl* uav)
{
	char timeStamp[10] = {0};
	time_t ltime = time(NULL);
	strftime(timeStamp, sizeof(timeStamp), "%T", localtime(&ltime));

	logFile << setw(15) << left <<  timeStamp;
    logFile <<  setw(15) <<	-insect->getRelPosition().y/SPEED_SCALE;
    logFile <<  setw(15) <<	insect->getRelPosition().x/SPEED_SCALE;
    logFile <<  setw(15) <<	insect->isFound();
    logFile <<  setw(15) <<	int(uav->api.current_messages.battery_status.battery_remaining);
//    logFile <<	uav->api.current_messages.sys_status <<  "		";
    logFile <<  setw(15) <<	uav->api.current_messages.local_position_ned.x;
    logFile <<  setw(15) <<	uav->api.current_messages.local_position_ned.y;
    logFile <<  setw(15) <<	uav->api.current_messages.attitude.yaw;
    logFile <<  endl;
}




// ------------------------------------------------------------------------------
//   Show Image
// ------------------------------------------------------------------------------

void showImage (Mat image, Mat image_ROI, Insect insect) {

		if (contDebugFull) 
		{
		
			rectangle(image, insect.getROI(), Scalar(255, 255, 255), 2, 8, 0);
			line(image, Point(image.cols / 2, image.rows / 2), insect.getPosition(), Scalar(255, 0, 0), 3);
			line(image, insect.getPosition(), insect.getPosition() + 5*insect.getVelocity(), Scalar(0, 255, 0), 3);

			resize(image, image, Size(), 0.3, 0.3);
			//resize(image_ROI, image_ROI, Size(), 0.3, 0.3);

			imshow("Program Output", image);
		}
		else { destroyWindow("Program Output"); }

		if (contDebugROI && insect.isFound()) {
			resize(image_ROI, image_ROI, Size(), 0.5, 0.5);
			imshow("ROI", image_ROI);
		}
		else if (contDebugROI)
		{
			resize(image_ROI, image_ROI, Size(), 0.1, 0.1);
			imshow("ROI", image_ROI);
		}
		else { destroyWindow("ROI"); }

}



// ------------------------------------------------------------------------------
//   Main Program
// ------------------------------------------------------------------------------

void mainProgram(void) 
{
	// A method of measuring the FPS for the program
	struct timespec time1, time2;

	// The two main image matrices
	Mat src, src_ROI;

	// Ensures the screen is initially printed to the screen
	contRefresh = true;

	// Creates the UAV object
	UAVControl uav;
	uav.setPID(P_GAIN, I_GAIN, D_GAIN);

	// Creates the camera object
	Camera_Interface cam(&src);

	// Creates the insect object
	Insect insect(cam.getImageSize());

	// Set up the output log file
	
    createLog();
	
	
	while (!contQuit) {
		clock_gettime(CLOCK_REALTIME, &time1);

		// ---------------------------------------------------------------------
		//   UAV Control
		// ---------------------------------------------------------------------

	
		if (!uav.isInit() && contUAV) 
		{
			try { uav.init(); }

			catch (char const* s) 
			{ 
				wprintw(output.outputStream, "UAV init failed at: %s\n", s);
				wprintw(output.outputStream, "Press 'u' to try again\n\n");
				contUAV = false; 
			}
		}
		
		

		if (uav.isInit() && contUAV && insect.isFound()) 
		{	
			uav.updateVelocityPID(-insect.getRelPosition().y/SPEED_SCALE, insect.getRelPosition().x/SPEED_SCALE, 0);
		}
		else if (uav.isInit()) { uav.updateVelocity(0, 0, 0); }
	

		// ------------------------------------------------------------------
		//   Camera Control
		// ------------------------------------------------------------------

		if (!cam.isInit() && contCam) 
		{
			try 
			{ 
				cam.init(); 
				wprintw(output.outputStream, "Camera Connected\n");
			}
			catch (char const* s) 
			{ 
				wprintw(output.outputStream, "Cam init failed at: %s\n", s);
				wprintw(output.outputStream, "Press 'c' to try again\n\n");
				contCam = false; 
			}
		}

		if (cam.isInit() && contCam) 
		{

			try { cam.getImage(); }

			catch (char const* s) { wprintw(output.outputStream, "Cam capture failed: %s\n", s); }


			insect.updateROI();
			src_ROI = src(insect.getROI());
			insect.findInsect(&src_ROI);
			
	
			showImage(src, src_ROI, insect);
			
		}

		// ------------------------------------------------------------------
		//   Output
		// ------------------------------------------------------------------
	
		if (contRefresh) 
		{
			printMainScreen();
			insect.printParameters(INSECT_DATA_Y, INSECT_DATA_X);
			uav.printParameters(UAV_DATA_Y, UAV_DATA_X);
			cam.printParameters(CAMERA_DATA_Y, CAMERA_DATA_X);
			printFlagParameters(FLAG_DATA_Y, FLAG_DATA_X);
			
			contRefresh = false;
		}

		// Prints the line position
		wprintw(output.outputStream, ">\r");

		// Update the data outputs on every iteration
		cam.printOutput();
		uav.printOutput();
		insect.printOutput();
		printFlagOutput();
	
		// Refresh all data windows
		output.refresh();

		waitKey(WAIT_PERIOD);		// Must have this line in!
	
        if (uav.isInit()) { updateLog(&insect, &uav); }

		// ------------------------------------------------------------------
		//   Input
		// ------------------------------------------------------------------

		// Checks for user input
		inputControl();

		clock_gettime(CLOCK_REALTIME, &time2);
		double timeDiff = (time2.tv_sec - time1.tv_sec) + (time2.tv_nsec - time1.tv_nsec) / double(BILLION);
			
		cam.updateFPS(1/(timeDiff));
	}
	logFile.close();
}






/** @function main */
int main(int argc, char** argv)
{
	// Initialises the output screen for the UI
	output.init();	

	// The main program is run as a function to allow for natural class destruction on exit
	mainProgram();

	// Program does not immediately close to give the user the ability to view the final state of the program
	wprintw(output.outputStream, "\nProgram has quit. Press any key to exit"); 
	wrefresh(output.outputStream);

	getch();

	endwin();

	return(0);
}
