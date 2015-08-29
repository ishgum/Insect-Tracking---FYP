#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <stdio.h>

//#include <time.h>

#include <opencv2/gpu/gpu.hpp>

#include "Thresholding.h"
#include "Insect.h"
#include "Fps.h"
//#include "IrCam.h"
//#include "Kalman.h"

using namespace cv;
using namespace std;
using namespace cv::gpu;

#define DEBUG		//display video output windows
//#define FPS //wall breaks (==0) on release mode.
//#define KALMAN
#define WAIT_PERIOD	10
//#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage
//#define USE_GPU  // ALSO NEED TO CHANGE IN THRESHOLDING.H - #DEFINE THRESH_GPU


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

#ifdef USE_GPU
vector<Point2f> findObjects(GpuMat inputImage) {
	vector<vector<Point> > contours;
	vector<Point2f> centres;
	vector<Vec4i> hierarchy;
	GpuMat contourEdges_g;

	// Detect edges on GPU
	Canny(inputImage, contourEdges_g, 100, 100 * 2, 3);

	// Download edges to CPU (no current GPU findContours function)
	Mat contourEdges(contourEdges_g);

	findContours(contourEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	for (int i = 0; i < contours.size(); i++)
	{
		Moments conMom = (moments(contours[i], false));
		if ((conMom.m00 < 500) && (conMom.m00 > 5)) {
			centres.push_back(Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00));
		}
	}
	return centres;
}

#else
vector<Point2f> findObjects(Mat inputImage) {
	vector<vector<Point> > contours;
	vector<Point2f> centres;
	vector<Vec4i> hierarchy;
	Mat contourEdges;

	Canny(inputImage, contourEdges, 100, 100 * 2, 3);
	findContours(contourEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	for (int i = 0; i < contours.size(); i++)
	{
		Moments conMom = (moments(contours[i], false));
		if ((conMom.m00 > 5)) {
			centres.push_back(Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00));
		}
	}
	return centres;
}
#endif

#ifdef USE_GPU
Insect findInsect(Insect insect, GpuMat inputImage) {
    GpuMat image_hsl_g, lum_g, values_g[3];
	//Mat values[3], image_hsl, lum;

	//cvtColor(src, image_hsl, CV_BGR2HLS);		// Convert image to HSL - redundant for IR
	//split(*inputImage, values);						// Split into channels
    split(inputImage, values_g);				// GPU split into three channels; may need to be & of vector<GpuMat>
    
	lum_g = values_g[0];
	//Mat lum(values_g[0]);						// Download luminance channel off GPU

	int lumThreshold = findThreshold(lum_g);		//Perform Dynamic thresholding on the saturation image

	if (lumThreshold < 0) {
		insect.found = false;
		return insect;
	}

	//threshold(lum, lum, lumThreshold, 255, 0);
	threshold(lum_g, lum_g, lumThreshold, 255, 0);
	insect.updateHeight(lumThreshold);

	vector<Point2f> objectCentres = findObjects(lum_g);

	if (objectCentres.size() == 0) {
		insect.found = false;
		return insect;
	}

	insect.found = true;
	insect.updatePosition(objectCentres[0]);
	return insect;
}

#else

Insect findInsect(Insect insect, Mat* inputImage) {
	Mat values[3], image_hsl, lum, lumMorph;

	cvtColor(*inputImage, image_hsl, CV_BGR2HLS);		// Convert image to HSL - redundant for IR
	//split(*inputImage, values);
	split(image_hsl, values);						// Split into channels
	lum = values[1]; //0

	imshow("Luminance", lum);
	int lumThreshold = findThreshold(lum);		//Perform Dynamic thresholding on the saturation image

	if (lumThreshold == 0) {
		insect.found = false;
		return insect;
	}

	threshold(lum, lum, lumThreshold, 255, 0);
	imshow("Thresholded image", lum);
	insect.updateHeight(lumThreshold);

	Mat element = getStructuringElement(0, Size(4,4));
	morphologyEx(lum, lumMorph, 3, element);
	imshow("Thresholded image - Morph", lumMorph);

	vector<Point2f> objectCentres = findObjects(lumMorph);

	if (objectCentres.size() == 0) {
		insect.found = false;
		return insect;
	}

	insect.found = true;
	insect.updatePosition(objectCentres[0]);
	return insect;
}

#endif

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
	VideoCapture capture(0);

	#ifdef FPS
	Fps fps(WAIT_PERIOD, WALL); // set what displays by changing mode to WALL, CPU, or BOTH
	#endif // FPS

	Mat src, src_ROI;
	#ifdef USE_GPU
		GpuMat g_src;
	#endif

	capture >> src;
	
	Insect insect(&src);



	/********** WHILE LOOP *********/
//timespec time1, time2, time_diff;
	while (!src.empty()) {
		//imshow("Img", src);
		
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);

		src_ROI = src(insect.ROI);
		#ifdef USE_GPU
			g_src.upload(src_ROI);
			insect = findInsect(insect, g_src);

		#else
			insect = findInsect(insect, &src);
		#endif

		//insect.updateROI(&src);

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
		//printf("\n");
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
		//time_diff = diff(time1, time2);
		//cout << time_diff.tv_sec << ":" << time_diff.tv_nsec << endl;
	}
	cout << "Done\n";

	capture.release();

	return(0);
}
