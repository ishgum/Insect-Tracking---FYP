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
#include "mavlink_control.h"


using namespace cv;
using namespace std;

#define WAIT_PERIOD	10



#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage


bool runProgram;
char  c;

static bool contPause, contCam, contQuit, contUAV, contDebug;



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

	char currentInput = input.getChar();
	switch (currentInput) {
		case 'p':
			contPause = !contPause;
			break;
		
		case 'c':
			if (!contCam) printf("Camera started");
			contCam = true;
			break;


		case 'q':
			contQuit = true;
			break;

		case 'u':
			contUAV = !contUAV;
			break;

		case 'd':
			contDebug = !contDebug;
			if (contDebug) printf("\nDebug Operating\n");
			else printf("\nShutting debug off\n");
			break;
	}
}


void showImage (Mat image, Mat image_ROI, Insect insect) {

		if (contDebug) {
		/*
		rectangle(image, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
		line(image, Point(image.cols / 2, image.rows / 2), insect.position, Scalar(255, 0, 0), 3);
		line(image, insect.position, insect.position + 5*insect.velocity, Scalar(0, 255, 0), 3);

		resize(image, image, Size(), 0.3, 0.3);
		resize(image_ROI, image_ROI, Size(), 0.3, 0.3);

		imshow("Program Output", image);
		*/
		namedWindow("ROI");
		moveWindow("ROI", 1024, 10);
		imshow("ROI", image_ROI);
		}
		else { destroyAllWindows(); }

}


void printData (Insect insect, float fps) {
	if (insect.found) 
	{ 
		printf("\r X: %.2f  Y: %.2f  Angle: %.2f  FPS: %.2f ", insect.relPosition.x, insect.relPosition.y, insect.relAngle, fps); 
	}
	else { printf("\nNo insect found"); }
	return;
}





/** @function main */
int main(int argc, char** argv)
{

	int h, w;
   	getmaxyx(stdscr, h, w);
	WINDOW * win = newwin(nlines, ncols, y0, x0);
	

	clock_t time1, time2;
	Mat src, src_ROI;
	float fps = 0;

	input.start();

	UAVControl uav;
	uav.init();
	

	IRCam cam(&src);
	try { cam.init(); }
	catch (char const* s) 
	{ 
		cout << "Cam init failed at: " << s << endl; 
		return 0; 
	}

	cam.getImage();
	cout << "Got IR image. Size is " << src.size() << endl;

	Insect insect(&src);
	

	signal(SIGINT,quit_handler);
	
	pthread_t controlThread;
	//pthread_create(&controlThread, NULL, inputHandler, NULL);

	//initscr();
	cout << "Camera connected." << endl;
	cout << "Press c to start camera capture" << endl;
	cout << "Press d to toggle the image" << endl;
	cout << "Press u to toggle UAV control" << endl;

	//contCam = true;


	while (!contQuit) {

		mvprintw(0,0,"Hello");
		inputControl();
		
		if (contUAV) 
		{
			uav.toggleControl();
			uav.printControl();
			contUAV = false;
		}

		
		if (contCam) 
		{
			time1 = clock();
			

			src_ROI = src(insect.ROI);
			insect.findInsect(&src_ROI);
			insect.updateROI(&src);	

			printData(insect, fps);	
			showImage(src, src_ROI, insect);
			waitKey(10);

			cam.getImage();	
			time2 = clock();

			fps = findFPS(time1, time2);
		}

		if (uav.getControlStatus()) 
		{
			uav.updateVelocity(float(.002*insect.relPosition.x), -float(.002*insect.relPosition.y));
			uav.printPosition(); 
		}

		refresh();

	}

	endwin();
	cout << "\nDone\n";

	return(0);
}
