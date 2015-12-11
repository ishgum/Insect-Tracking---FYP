#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>

#include <time.h>
#include <signal.h>

#include <ncurses.h>

//#include "Thresholding.h"
#include "Insect.h"
#include "IrCam.h"
#include "Input.h"
#include "Output.h"
//#include "OutputStream.h"
#include "mavlink_control.h"


using namespace cv;
using namespace std;

#define WAIT_PERIOD	10



#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage


bool runProgram;
char  c;

static bool contPause, contCam, contQuit, contUAV, contDebugROI, contDebugFull;



void printFlags(void) {

	werase(output.flagData);
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



void inputControl(void) {

	char currentInput = wgetch(output.outputStream);
	switch (currentInput) {
		case 'p':
			contPause = !contPause;
			break;
		
		case 'c':
			contCam = true;
			break;


		case 'q':
			contQuit = true;
			break;

		case 'u':
			contUAV = !contUAV;
			break;

		case 'd':
			contDebugROI = !contDebugROI;
			break;

		case 'f':
			contDebugFull = !contDebugFull;
			break;
	}

}


void showImage (Mat image, Mat image_ROI, Insect insect) {

		if (contDebugFull) {
		
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


void printData (Insect insect, float fps) {
	if (insect.found) 
	{ 
		printf("\r X: %.2f  Y: %.2f  Angle: %.2f  FPS: %.2f ", insect.relPosition.x, insect.relPosition.y, insect.relAngle, fps); 
	}
	else { printf("\nNo insect found"); }
	return;
}



void mainProgram(void) {
	clock_t time1, time2;
	Mat src, src_ROI;


	UAVControl uav;
	IRCam cam(&src);
	Insect insect(cam.getImageSize());

	wprintw(output.outputStream, "Press c to start camera capture\n");
	wprintw(output.outputStream, "Press d to toggle the image\n");
	wprintw(output.outputStream, "Press u to toggle UAV control\n");


	while (!contQuit) {

		inputControl();

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

		
		if (uav.isInit() && contUAV) 
		{
			uav.toggleControl();
			uav.printControl();
			contUAV = false;
		}
	
	
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
			
			//printData(insect, fps);	
			showImage(src, src_ROI, insect);
			waitKey(10);
			
			time2 = clock();

			cam.updateFPS(findFPS(time1, time2));
		}

		if (insect.found && uav.isInit()) {
			uav.updateVelocity(insect.relPosition.x, insect.relPosition.y, 0);
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
    initscr();
	cbreak(); 
    noecho();
	curs_set(0);
	
	 
    printMainScreen();

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
