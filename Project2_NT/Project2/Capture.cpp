// display the difference between adjacent video frames (press any key to exit)
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//using namespace cv;

/*
int main()
{
	Mat image;
	VideoCapture capture(0); // connect to a camera
	
	int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoWriter video("C:/Users/myadmin/Videos/retroFar1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 15, Size(frame_width, frame_height), true);

	while (waitKey(10) < 0) {
		capture >> image;
		imshow("Image", image);
	}

	while (waitKey(10) < 0) {
		capture >> image;
		video.write(image);
		
		imshow("Image", image);
	}
	video.release();
	return 0;
}
*/


/**
* OpenCV SimpleBlobDetector Example
*
* Copyright 2015 by Satya Mallick <spmallick@gmail.com>
*
*/

//#define CAP
#ifdef CAP

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


using namespace cv;
using namespace std;

Mat src; Mat src_hsl; Mat dst;

int main(int argc, char** argv)
{
	VideoCapture capture;

	capture.open("C:/Users/myadmin/Videos/retroHigh1.avi");

	capture >> src;

	while (!src.empty() && waitKey(10) < 0) {
		

		namedWindow("Raw video", CV_WINDOW_AUTOSIZE);
		imshow("Raw video", src);

		Mat values[3];

		// Convert image to HSL
		cvtColor(src, src_hsl, CV_BGR2HLS);


		// Split into channels
		split(src_hsl, values);

		Mat lum = values[1];

		//Mat lum = imread("C:/Users/myadmin/Documents/Image/blob.jpg", IMREAD_GRAYSCALE);

		medianBlur(lum, dst, 5);

		//threshold(dst, dst, 250, 255, 0);
		adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, 1, 11, 10);

		int morph_size = 4;
		Mat element = getStructuringElement(2, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		/// Apply the specified morphology operation
		morphologyEx(dst, dst, 1, element);
		//imshow("Feed", dst);

		// Read image
		//Mat im = cvQueryFrame(capture);

		imshow("Image", dst);
	

		capture >> src;

	}

	capture.release();
	return(0);
}


#endif






// Blob detector

/*

// Setup SimpleBlobDetector parameters.
SimpleBlobDetector::Params params;

//params.blobColor = 255;

// Change thresholds
params.minThreshold = 200;
params.maxThreshold = 255;

// Filter by Area.
params.filterByArea = true;
params.minArea = 1;

// Filter by Circularity
params.filterByCircularity = false;
params.minCircularity = 0.9;

// Filter by Convexity
params.filterByConvexity = false;
params.minConvexity = 0.87;

// Filter by Inertia
params.filterByInertia = false;
params.minInertiaRatio = 0.5;


// Storage for blobs
vector<KeyPoint> keypoints;

// Set up detector with params
SimpleBlobDetector detector(params);

// Detect blobs;

detector.detect(dst, keypoints);


// Draw detected blobs as red circles.
// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
// the size of the circle corresponds to the size of blob

Mat im_with_keypoints;
drawKeypoints(src, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

// Show blobs
imshow("keypoints", im_with_keypoints);

*/