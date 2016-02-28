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

#include <time.h>
#include <signal.h>

#include <ncurses.h>

#include "insect.h"
#include "camera_interface.h"
#include "common.h"
#include "mavlink_interface.h"
#include "ui_input.h"


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

#define WAIT_PERIOD	10




// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------


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

		if (contDebugROI) {
			namedWindow("ROI");
			moveWindow("ROI", 1024, 10);
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
	clock_t time1, time2;

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


	while (!contQuit) {

		// Checks for user input
		inputControl();


		// ------------------------------------------------------------------------------
		//   UAV Control
		// ------------------------------------------------------------------------------

		
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

		else if (uav.isInit() && contTest)
		{
			for (int i = 0; i < 100; i++)
			{
				switch (i%8)
				{
					case 0:
						uav.updateVelocityPID(CRAFT_SPEED, 0, 0);
						wprintw(output.outputStream, "\nNorth");
						break;

					case 1:
						uav.updateVelocityPID(CRAFT_SPEED, CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nNorth East");
						break;

					case 2:
						uav.updateVelocityPID(0, CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nEast");
						break;

					case 3:
						uav.updateVelocityPID(-CRAFT_SPEED, CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nSouth East");
						break;

					case 4:
						uav.updateVelocityPID(-CRAFT_SPEED, 0, 0);
						wprintw(output.outputStream, "\nSouth");
						break;

					case 5:
						uav.updateVelocityPID(-CRAFT_SPEED, -CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nSouth West");
						break;

					case 6:
						uav.updateVelocityPID(0, -CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nWest");
						break;

					case 7:
						uav.updateVelocityPID(CRAFT_SPEED, -CRAFT_SPEED, 0);
						wprintw(output.outputStream, "\nNorth West");
						break;
				}
				wrefresh(output.outputStream);
				sleep(CRAFT_TIME);
			}
		}

		else if (uav.isInit()) { uav.updateVelocity(0, 0, 0); }
	

		// ------------------------------------------------------------------------------
		//   Camera Control
		// ------------------------------------------------------------------------------

		if (!cam.isInit() && contCam) {
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
			time1 = clock();

			try { cam.getImage(); }

			catch (char const* s) { wprintw(output.outputStream, "Cam capture failed: %s\n", s); }


			insect.updateROI();
			src_ROI = src(insect.getROI());
			insect.findInsect(&src_ROI);
			
	
			showImage(src, src_ROI, insect);
			waitKey(WAIT_PERIOD);		// Must have this line in!
			
			time2 = clock();

			cam.updateFPS(cam.findFPS(time1, time2));
		}

		// ------------------------------------------------------------------------------
		//   Output
		// ------------------------------------------------------------------------------
	

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

	}

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
