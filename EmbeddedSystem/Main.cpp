#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>

#include <time.h>
#include <signal.h>

#include <ncurses.h>

#include "Insect.h"
#include "IrCam.h"
#include "Input.h"
#include "Output.h"
#include "mavlink_control.h"


using namespace cv;
using namespace std;

#define WAIT_PERIOD	10



bool runProgram;
char  c;

static bool contPause, contCam, contQuit, contUAV, contDebugROI, contDebugFull, contRefresh;



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


void printFlags(void) {

	werase(output.flagData);
	wprintw(output.flagData, "\n\n");
	wprintw(output.flagData, "%d\n", contDebugROI);
	wprintw(output.flagData, "%d\n", contDebugFull);
	wprintw(output.flagData, "%d\n", contPause);	
	wprintw(output.flagData, "%d\n", !contQuit);
}



void quit_handler (int sig)
{
	contQuit = true;
}





float findFPS(float t1, float t2)   
{   

	float diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC );   
	float fps = 1/diff;

	return fps;   
 }
 
 
 
 
 void drawMainWindow(void)
 {
 	erase();
 	int h, w;
	getmaxyx(stdscr, h, w);
	while (h < 42 || w < 89)
	{
		mvprintw(0,0,"Error: Screen is too small.\n");
		printw("Height = %d < 42\n", h);
		printw("Width = %d < 89", w);
		refresh();
		erase();
		getmaxyx(stdscr, h, w);
	}
	
	resizeterm(42, 89);
	refresh();

    printMainScreen();
}






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


void showImage (Mat image, Mat image_ROI, Insect insect) {

		if (contDebugFull) 
		{
		
			rectangle(image, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
			line(image, Point(image.cols / 2, image.rows / 2), insect.position, Scalar(255, 0, 0), 3);
			line(image, insect.position, insect.position + 5*insect.velocity, Scalar(0, 255, 0), 3);

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



void mainProgram(void) {
	clock_t time1, time2;
	Mat src, src_ROI;
	contRefresh = true;


	UAVControl uav;
	IRCam cam(&src);
	Insect insect(cam.getImageSize());


	while (!contQuit) {

		inputControl();

		/* UAV Control */
		
		
		if (contRefresh) 
		{
			drawMainWindow();
			insect.printParameters(INSECT_DATA_Y, INSECT_DATA_X);
			uav.printParameters(UAV_DATA_Y, UAV_DATA_X);
			cam.printParameters(CAMERA_DATA_Y, CAMERA_DATA_X);
			printFlagParameters(FLAG_DATA_Y, FLAG_DATA_X);
			
			contRefresh = false;
		}
		
		
		if (!uav.isInit() && contUAV) {
			try 
			{           
				uav.init(); 
				wprintw(output.outputStream, "UAV Connected\n");
}
			catch (char const* s) 
			{ 
				wprintw(output.outputStream, "UAV init failed at: %s\n", s);
				wprintw(output.outputStream, "Press 'u' to try again\n\n");
				contUAV = false; 
			}
		}

		
		if (uav.isInit() && contUAV && insect.found) 
		{
			uav.updateVelocity(insect.relPosition.x/SPEED_SCALE, insect.relPosition.y/SPEED_SCALE, 0);
		}
		else if (uav.isInit()) { uav.updateVelocity(0, 0, 0); }
	

		/* Camera Control */

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
			catch (char const* s) 
			{ wprintw(output.outputStream, "Cam capture failed: %s\n", s); }


			insect.updateROI();
			src_ROI = src(insect.ROI);
			insect.findInsect(&src_ROI);
			
	
			showImage(src, src_ROI, insect);
			waitKey(10);
			
			time2 = clock();

			cam.updateFPS(findFPS(time1, time2));
		}

	
		wprintw(output.outputStream, ">\r");

		cam.printOutput();
		uav.printOutput();
		insect.printOutput();
		printFlags();
	
		output.refresh();

	}

}






/** @function main */
int main(int argc, char** argv)
{
	signal(SIGINT,quit_handler);
	printf("Got here");
	initscr();
	cbreak(); 
	noecho();
	curs_set(0);
	
	output.init();
	nodelay(output.outputStream, TRUE);
	

	mainProgram();

	wprintw(output.outputStream, "\n\n\n\nProgram has quit. Press any key to exit"); 
	wrefresh(output.outputStream);

	getch();

    endwin();
/*
	
	
	pthread_t controlThread;
	//pthread_create(&controlThread, NULL, inputHandler, NULL);

	//initscr();
	cout << "Camera connected." << endl;
	cout << "Press c to start camera capture" << endl;
	cout << "Press d to toggle the image" << endl;
	cout << "Press u to toggle UAV control" << endl;

	//contCam = true;


	while (!contQuit) {


		


		if (uav.getControlStatus()) 
		{
			uav.updateVelocity(float(.002*insect.relPosition.x), -float(.002*insect.relPosition.y));
			uav.printPosition(); 
		}

		refresh();

	}

	endwin();
	cout << "\nDone\n";
*/
	return(0);
}
