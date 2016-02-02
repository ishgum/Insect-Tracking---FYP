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

using namespace cv;
using namespace std;

#define WAIT_PERIOD	10



bool runProgram;





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



void mainProgram(void) {
	clock_t time1, time2;
	Mat src, src_ROI;
	contRefresh = true;


	UAVControl uav;
	uav.setPID(1, 0, 0);
	Camera_Interface cam(&src);
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

		
		if (uav.isInit() && contUAV && insect.isFound()) 
		{
			uav.updateVelocityPID(insect.getRelPosition().x/SPEED_SCALE, insect.getRelPosition().y/SPEED_SCALE, 0);
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
			src_ROI = src(insect.getROI());
			insect.findInsect(&src_ROI);
			
	
			showImage(src, src_ROI, insect);
			waitKey(WAIT_PERIOD);		// Must have this line in!
			
			time2 = clock();

			cam.updateFPS(findFPS(time1, time2));
		}

	
		wprintw(output.outputStream, ">\r");

		cam.printOutput();
		uav.printOutput();
		insect.printOutput();
		printFlagOutput();
	
		output.refresh();

	}

}






/** @function main */
int main(int argc, char** argv)
{
//	signal(SIGINT,quit_handler);
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
