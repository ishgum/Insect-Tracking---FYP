#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>

#include "Thresholding.h"
#include "Insect.h"
#include "Fps.h"
#include "IrCam.h"
#include "Kalman.h"

using namespace cv;
using namespace std;

#define ROI_SIZE .15
#define DEBUG		//display video output windows
#define FPS //wall breaks (==0) on release mode. !When FPS defined && DEBUG undefined release mode breaks
#define KALMAN
#define HEIGHT_OFFSET 10
#define WAIT_PERIOD	10
//#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}


Rect updateROI(Rect ROI, Point2f stateLoc, Mat src) {
	int roiSize = ROI_SIZE * src.rows;

	if (stateLoc.x > (roiSize / 2)) {
		ROI.x = int(stateLoc.x) - roiSize / 2;
	}
	if (stateLoc.y > (roiSize / 2)) {
		ROI.y = int(stateLoc.y) - roiSize / 2;
	}
	if (stateLoc.x + roiSize / 2 > src.cols) {
		ROI.x = src.cols - roiSize;
	}
	if (stateLoc.y + roiSize / 2 > src.rows) {
		ROI.y = src.rows - roiSize;
	}
	ROI.width = roiSize;
	ROI.height = roiSize;
	return ROI;
}


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
		if ((conMom.m00 < 500) && (conMom.m00 > 5)) {
			centres.push_back(Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00));
		}
	}
	return centres;
}


Insect findInsect(Mat inputImage, Insect insect, Rect ROI) {
	Mat values[3], image_hsl, lum;

	//cvtColor(src, image_hsl, CV_BGR2HLS);		// Convert image to HSL
	split(inputImage, values);						// Split into channels
	lum = values[0];

	int lumThreshold = findThreshold(lum);		//Perform Dynamic thresholding on the saturation image

	if (lumThreshold < 0) {
		insect.found = false;
		return insect;
	}

	threshold(lum, lum, lumThreshold, 255, 0);
	insect.updateHeight(lumThreshold);

	vector<Point2f> objectCentres = findObjects(lum);

	if (objectCentres.size() == 0) {
		insect.found = false;
		return insect;
	}

	insect.found = true;
	insect.updatePosition(objectCentres[0], ROI);
	return insect;
}


/** @function main */
int main(int argc, char** argv)
{
	VideoCapture capture;
#ifdef USE_CAM
	if(!irCamInit()) {
		cout << "\nFailed to connect to camera. Aborting.\n";
		return -1;
	}
#else
	//EARLY TESTS:
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
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun2_0.avi");

	// Relative path to small test file
	capture.open("../../test.avi");

	//DYLANS folder structure:
	//capture.open("C:/Users/Dylan/Documents/FYP/data/MVI_2987.MOV");
#endif

	#ifdef RECORD_SOURCE_W_BOX
		int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		int input_fps = capture.get(CV_CAP_PROP_FPS);
		input_fps = 25;
		VideoWriter outputVideo("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), input_fps, Size(frame_width, frame_height), true);
	#endif

	#ifdef KALMAN
		KalmanFilter KF = kalmanInit();
		#ifdef DEBUG
			vector<Point> targetv, kalmanv;
		#endif //DEBUG
	#endif    // KALMAN

	#ifdef FPS
		float fps = 0;
		fps = checkFPS(WAIT_PERIOD);
	#endif // FPS

	Mat src, src_ROI;
	Insect insect;

#ifdef USE_CAM
	src = irGetImage();
#else
	capture >> src;
#endif

	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame


	/********** WHILE LOOP *********/
	while (!src.empty()) {
#ifdef FPS
		fps = checkFPS(WAIT_PERIOD);
		displayFPS(src, ROI, fps);
#endif // FPS

#ifdef RECORD_SOURCE_W_BOX
		// write output video w/ text
		outputVideo.write(src_w_text);
#endif

		src_ROI = src(ROI);

		insect = findInsect(src_ROI, insect, ROI);

		if (insect.found) {
			ROI = updateROI(ROI, insect.position, src);
		}
		else {
			ROI = Rect(0, HEIGHT_OFFSET, src.cols, src.rows - HEIGHT_OFFSET); //Reset ROI
		}

#ifdef DEBUG
		Mat srcBox = src.clone();
		rectangle(srcBox, ROI, Scalar(255, 255, 255), 2, 8, 0);
		imshow("Source w Box", srcBox);
		imshow("Frame", src_ROI);
		//imshow("Luminance", lum);
		printf("Height Bracket: %i	", insect.heightBracket);

		/// Draw contours
		Mat contourOutput = Mat::zeros(src_ROI.size(), CV_8UC3);
		//for (int i = 0; i < objectCentres.size(); i++)
		//{
		//	circle(contourOutput, objectCentres[i], 4, Scalar(255, 0, 0), -1, 8, 0);
		//}
		//imshow("Contours", contourOutput);
		printf("Speed: %.1f	", insect.speed);
#endif // DEBUG


#ifdef KALMAN
		kalmanProcess(&KF, &insect);	
	#ifdef DEBUG
		// Plot Kalman info
		Point2f stateLoc = kalmanGetStateLoc();
		Point2f measLoc = kalmanGetMeasLoc();

		// Only run in debug mode to avoid consuming a large amount of memory
		targetv.push_back(measLoc);
		kalmanv.push_back(stateLoc);
		src = Scalar::all(0);

		drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
		drawCross(src, measLoc, Scalar(0, 0, 255), 5);

		for (int i = 0; i < targetv.size() - 1; i++)
			line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

		for (int i = 0; i < kalmanv.size() - 1; i++)
			line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

		imshow("Frame Kalman", src);
	#endif //DEBUG
#endif //KALMAN

#ifdef USE_CAM
		src = irGetImage();
#else
		capture >> src;
#endif
		//resize(src, src, Size(), 0.3, 0.3);

		waitKey(WAIT_PERIOD);
		printf("\n");
	}

	cout << "Done\n";

#ifdef USE_CAM
	irReleaseCam();
#else
	capture.release();
#endif

	return(0);
}