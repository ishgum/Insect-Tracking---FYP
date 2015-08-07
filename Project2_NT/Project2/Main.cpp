#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>

// GPU libs

#include <opencv2/gpu/gpu.hpp

#include "Thresholding.h"
#include "Insect.h"
#include "Fps.h"
#include "IrCam.h"
#include "Kalman.h"

using namespace cv;
using namespace std;

using namespace cv::gpu;

#define DEBUG		//display video output windows
#define FPS //wall breaks (==0) on release mode.
//#define KALMAN
#define WAIT_PERIOD	10
#define USE_CAM		// On to use IR cam (real-time), off to use recorded footage


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
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


Insect findInsect(Insect insect, Mat* inputImage) {
	Mat values[3], image_hsl, lum;

	//cvtColor(src, image_hsl, CV_BGR2HLS);		// Convert image to HSL - redundant for IR
	split(*inputImage, values);						// Split into channels
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
	insect.updatePosition(objectCentres[0]);
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
	capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun2_0.avi");

	// Relative path to small test file
	//capture.open("../../test.avi");

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
	Fps fps(WAIT_PERIOD, fps_mode_t::WALL); // set what displays by changing mode to WALL, CPU, or BOTH
	#endif // FPS

	Mat src, src_ROI;
	GpuMat g_src;

#ifdef USE_CAM
	src = irGetImage();
#else
	capture >> src;
#endif
	
	Insect insect(&src);




	/********** WHILE LOOP *********/


	while (!src.empty()) {
        
        //Upload frame to gpu (TO DO: investigate direct stream to GPU mem)
        g_src.upload(src);  //Need to download to CPU once processing finished - g_src.download(src)

#ifdef RECORD_SOURCE_W_BOX
		// write output video w/ text
		outputVideo.write(src_w_text);
#endif

		src_ROI = src(insect.ROI);

		insect = findInsect(insect, &src_ROI);
		insect.updateROI(&src);

#ifdef DEBUG
		Mat srcBox = src.clone();
		rectangle(srcBox, insect.ROI, Scalar(255, 255, 255), 2, 8, 0);
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
		printf("Angle: %.0f", insect.relAngle);
		Mat insectPosition = src.clone();
		line(insectPosition, Point(src.cols / 2, src.rows / 2), insect.position, Scalar(255, 0, 0), 3);
		line(insectPosition, insect.position, insect.position + 5*insect.velocity, Scalar(0, 255, 0), 3);
		imshow("Insect Position", insectPosition);
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

#ifdef FPS
		fps.checkFPS();
		fps.displayFPS(src, insect.ROI);
#endif // FPS

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