#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>


#include "Thresholding.h"
#include "Insect.h"
#include "Fps.h"
//#include "IrCam.h"
//#include "Kalman.h"

using namespace cv;
using namespace std;

#define DEBUG		//display video output windows
//#define FPS //wall breaks (==0) on release mode.
#define WAIT_PERIOD	10
//#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage



RNG rng(12345);


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

vector<vector<Point> > findObjects(Mat* inputImage) {
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat contourEdges, contourMorph;

	Canny(*inputImage, contourEdges, 30, 50 * 2, 3);

	Mat element = getStructuringElement(0, Size(5, 5));
	morphologyEx(contourEdges, contourMorph, 3, element);

	findContours(contourMorph, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	return contours;
}


map<double, vector<Point>> mapContours(vector<vector<Point>> inputContours, Mat inputImage) {
	map<double, vector<Point>> contourMap;

	for (int i = 0; i < inputContours.size(); i++)
	{
		Mat mask = Mat::zeros(inputImage.size(), CV_8UC1);
		drawContours(mask, inputContours, i, cv::Scalar(255), CV_FILLED, CV_AA, noArray(), 1, Point(0, 0));
		contourMap[mean(inputImage, mask)[0]] = inputContours[i];

	}

	return contourMap;
}



Insect findInsect(Insect insect, Mat* inputImage) {
	vector<vector<Point>> imageContours = findObjects(inputImage);
	map<double, vector<Point>> contourMap = mapContours(imageContours, *inputImage);

	if (contourMap.size() != 0) {
		double insectIntensity = contourMap.rbegin()->first;
		Moments conMom = (moments(contourMap.rbegin()->second, false));
		Point insectCentre = Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00);

		printf("Insect Position - X: %u, Y: %u.  Insect Height - %.2f \n", insectCentre.x, insectCentre.y, insectIntensity);
		insect.updatePosition(insectCentre);
		insect.found = true;
	}
	else {
		insect.found = false;
	}

	return insect;
}


/*
/*Determines time difference between two CPU clock times
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}
*/

/** @function main */
int main(int argc, char** argv)
{
	VideoCapture capture;

	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Ancient_times/plainHigh1.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2990.MOV"); //runs at ~6fps
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2987.MOV");

	//IR RREFLEC TESTS:
	//capture.open("C:/Users/myadmin/Documents/IR footage/retro2_2015-05-09-193310-0000.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only.avi"); 
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only_Uncompressed_Grayscale.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193006-0000_8bit_uncompressed.avi"); // Princess Beetle and the sparkly dress, Co-Staring Michael
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro1_2015-05-09-192708-0000.avi"); //persistent bright region on lower portion of frame

	//DEPTH TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun4_0.avi");



	#ifdef FPS
	Fps fps(WAIT_PERIOD, WALL); // set what displays by changing mode to WALL, CPU, or BOTH
	#endif // FPS


	Mat src, src_ROI;
	capture >> src;
	//resize(src, src, Size(), 0.3, 0.3);
	Insect insect(&src);
	
	while (!src.empty()) {

		src_ROI = src(insect.ROI);

		//imshow("Source", src);


		insect = findInsect(insect, &src_ROI);

		insect.updateROI(&src);

#ifdef DEBUG
		Mat srcBox = src.clone();
		rectangle(srcBox, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
		imshow("Source w Box", srcBox);
		imshow("Frame", src_ROI);
		//imshow("Luminance", lum);
		//printf("Height Bracket: %i	", insect.heightBracket);

		/// Draw contours
		Mat contourOutput = Mat::zeros(src_ROI.size(), CV_8UC3);
		//for (int i = 0; i < objectCentres.size(); i++)
		//{
		//	circle(contourOutput, objectCentres[i], 4, Scalar(255, 0, 0), -1, 8, 0);
		//}
		//imshow("Contours", contourOutput);
		//printf("Speed: %.1f	", insect.speed);
		//printf("Angle: %.0f", insect.relAngle);
		Mat insectPosition = src.clone();
		line(insectPosition, Point(src.cols / 2, src.rows / 2), insect.position, Scalar(255, 0, 0), 3);
		line(insectPosition, insect.position, insect.position + 5*insect.velocity, Scalar(0, 255, 0), 3);
		imshow("Insect Position", insectPosition);
#endif // DEBUG
		//Mat srcBox = src.clone();
		//rectangle(srcBox, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
		//imshow("Source w Box", srcBox);


#ifdef FPS
		fps.checkFPS();
		fps.displayFPS(src, insect.ROI);
#endif // FPS

		capture >> src;
		//resize(src, src, Size(), 0.3, 0.3);

		waitKey(WAIT_PERIOD);
	}
	cout << "Done\n";

	capture.release();

	return(0);
}
