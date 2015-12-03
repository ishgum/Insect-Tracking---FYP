#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>

#include <time.h>

#include <opencv2/gpu/gpu.hpp>

//#include "Thresholding.h"
#include "Insect.h"
#include "Fps.h"
#include "IrCam.h"


using namespace cv;
using namespace std;

#define WAIT_PERIOD	10


//#define DEBUG		//display video output windows
#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage



float findFPS(float t1, float t2)   
{   

	float diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC );   
	float fps = 1/diff;

	return fps;   
 }



/** @function main */
int main(int argc, char** argv)
{
	clock_t time1, time2;

	VideoCapture capture;
	cout << "Init Cam" << endl;
	if(!irCamInit()) {
		cout << "\nFailed to connect to camera. Aborting.\n";
		return -1;
	}
	cout << "Finished init cam" << endl;



	Mat src, src_ROI;
	src = irGetImage();
	cout << "Got IR image. Size is " << src.size() << endl;


	Insect insect(&src);

	

	/********** WHILE LOOP *********/
	cout << "Starting LOOP!" << endl;


	while (!src.empty() && 	waitKey(WAIT_PERIOD) < 0) {

		time1 = clock();
	
		src_ROI = src(insect.ROI);
		insect.findInsect(&src_ROI);
		insect.updateROI(&src);


		cout << "X: " << int(insect.position.x) << " Y: " << int(insect.position.y) << endl;
		if (insect.found) cout << "Insect Found!" << endl;

#ifdef DEBUG

		Mat srcBox = src.clone();
		rectangle(srcBox, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
		line(srcBox, Point(src.cols / 2, src.rows / 2), insect.position, Scalar(255, 0, 0), 3);
		line(srcBox, insect.position, insect.position + 5*insect.velocity, Scalar(0, 255, 0), 3);

		resize(srcBox, srcBox, Size(), 0.3, 0.3);
		resize(src_ROI, src_ROI, Size(), 0.3, 0.3);

		imshow("Program Output", srcBox);
		imshow("ROI", src_ROI);

		cout << " Height: " << insect.heightBracket;
		cout << " Speed: " << insect.speed;
		cout << " Angle: " << insect.relAngle;
#endif // DEBUG


		src = irGetImage();

		time2 = clock();
		cout << "FPS: " << findFPS(time1, time2) << endl;

	}
	cout << "Done\n";

	irReleaseCam();


	return(0);
}
