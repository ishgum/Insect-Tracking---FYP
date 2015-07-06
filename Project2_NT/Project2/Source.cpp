
#define RUN
#ifdef RUN


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>


#include "Thresholding.h"
#include "Insect.h"

using namespace cv;
using namespace std;


#define ROI_SIZE .15
#define DEBUG		//display video output windows
//#define FPS //wall breaks (==0) on release mode. !When FPS defined && DEBUG undefined release mode breaks
//#define KALMAN

#define HEIGHT_OFFSET 10




// Functions for system clock so we can determine runtine FPS
// copied from http://stackoverflow.com/questions/17432502
//  Windows
#ifdef _WIN32
#include <Windows.h>
double get_wall_time(){
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)){
		//  Handle error
		return 0;
	}
	if (!QueryPerformanceCounter(&time)){
		//  Handle error
		return 0;
	}
	return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time(){
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0){
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}
	else{
		//  Handle error
		return 0;
	}
}
//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
double get_wall_time(){
	struct timeval time;
	if (gettimeofday(&time, NULL)){
		//  Handle error
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
	return (double)clock() / CLOCKS_PER_SEC;
}
#endif



KalmanFilter setKalmanParameters(KalmanFilter KF) {
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1, 0, 10, 0,
		0, 1, 0, 10,
		0, 0, 1, 0,
		0, 0, 0, 1);

	KF.statePre.at<float>(0) = 0;
	KF.statePre.at<float>(1) = 0;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;

	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-3));
	setIdentity(KF.errorCovPost, Scalar::all(1));
	setIdentity(KF.errorCovPre, Scalar::all(1));
	return KF;
}


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}


Rect updateROI(Rect ROI, Point stateLoc, Mat src) {
	int roiSize = ROI_SIZE * src.rows;

	if (stateLoc.x > (roiSize / 2)) {
		ROI.x = stateLoc.x - roiSize / 2;
	}
	if (stateLoc.y > (roiSize / 2)) {
		ROI.y = stateLoc.y - roiSize / 2;
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



/** @function main */
int main(int argc, char** argv)
{

	VideoCapture capture;

	//EARLY TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Ancient_times/plainHigh1.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2990.MOV"); //runs at ~6fps
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2987.MOV");

	//IR RREFLEC TESTS:
	//capture.open("C:/Users/myadmin/Documents/IR footage/retro2_2015-05-09-193310-0000.avi");
//	capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only.avi"); 
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only_Uncompressed_Grayscale.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193006-0000_8bit_uncompressed.avi"); // Princess Beetle and the sparkly dress, Co-Staring Michael
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro1_2015-05-09-192708-0000.avi"); //persistent bright region on lower portion of frame

	//DEPTH TESTS:
	capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun2_0.avi");

	//DYLANS folder structure:
	//capture.open("C:/Users/Dylan/Documents/FYP/data/MVI_2987.MOV");

#ifdef RECORD_SOURCE_W_BOX
	int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	int input_fps = capture.get(CV_CAP_PROP_FPS);
	input_fps = 25;
	VideoWriter outputVideo("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), input_fps, Size(frame_width, frame_height), true);
#endif

#ifdef KALMAN
	KalmanFilter KF(4, 2, 0);
	Mat state(4, 1, CV_32F); //x, y, delta x, delta y
	Mat processNoise(4, 1, CV_32F);
	Mat measurement = Mat::zeros(2, 1, CV_32F);
	KF = setKalmanParameters(KF);
	vector<Point> targetv, kalmanv;
#endif

	int wait_period = 10;   /// Waitkey period

#ifdef FPS
	int frame_num = 1;
	// Timing functions
	double wall0 = get_wall_time();
	double cpu0 = get_cpu_time();
	int num_frames_proc = -1;
	float fps_wall, fps_cpu = 0;
	double cpu_running_total = 0;

#endif // FPS


	Mat src, src_ROI, values[3], image_hsl, lum;
	Insect insect;



	capture >> src;
	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame

	while (!src.empty()) {


#ifdef FPS
		num_frames_proc++;
		double wall1 = get_wall_time();
		double cpu1 = get_cpu_time();
		//cpu_running_total += (cpu1 - cpu0);

		//cpu0 = get_cpu_time();


		//printf("\t%f\t%f  ",wall1, wall1 - wall0);
		if (wall1 >= (wall0 + 1)){ //Check if 1s has elapsed
			//fps_wall_displayed = num_frames_proc / (wall1 - wall0); //rate displayed
			fps_wall = num_frames_proc / (wall1 - wall0 - num_frames_proc*0.001*wait_period);
			fps_cpu = num_frames_proc / (cpu1 - cpu0);// - num_frames_proc*0.001*wait_period);
			printf("\tFPS:  %f\t", fps_cpu);
			num_frames_proc = 0;
			wall0 = get_wall_time();
			cpu0 = get_cpu_time();
		}

		Mat src_w_text = src.clone();	// so we don't mess up original source
		rectangle(src_w_text, ROI, Scalar(255, 255, 255), 2, 8, 0);
		//resize(src_w_text, src_w_text, Size(), 0.3, 0.3);
		// add fps text to "1. Frame" window
		// Displayed FPS accounts for the delay we add in waitkey, far, far below.
		//wall time
		char fps_wall_c[4];
		sprintf(fps_wall_c, "wall FPS %.2f", fps_wall);
		Point fps_wall_text_loc(10, 30);
		putText(src_w_text, fps_wall_c, fps_wall_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);

		//cpu time
		char fps_cpu_c[4];
		sprintf(fps_cpu_c, "cpu FPS %.2f", fps_cpu);
		Point fps_cpu_text_loc(10, 60);
		putText(src_w_text, fps_cpu_c, fps_cpu_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);

		//Frame Number
		char frame_num_c[5];
		sprintf(frame_num_c, "Frame #: %5i", frame_num);
		Point frame_num_text_loc(10, 90);
		putText(src_w_text, frame_num_c, frame_num_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);
#ifdef DEBUG
		imshow("FPS", src_w_text);
#endif
		if ((frame_num % 100) == 0){ //show only every 100th frame
		//	imshow("FPS", src_w_text);
		}
		//printf("\nFrame: %i\tCPU total: %f\tCPU FPS: %f", frame_num, cpu_running_total, fps_cpu);
		frame_num++;

#ifdef RECORD_SOURCE_W_BOX
		// write output video w/ text
		outputVideo.write(src_w_text);
#endif
#endif // FPS	

		src_ROI = src(ROI);
		ROI = Rect(0, HEIGHT_OFFSET, src.cols, src.rows - HEIGHT_OFFSET); //Reset ROI


		//cvtColor(src, image_hsl, CV_BGR2HLS);		// Convert image to HSL
		split(src, values);						// Split into channels
		lum = values[0];

		int lumThreshold = findThreshold(lum);		//Perform Dynamic thresholding on the saturation image

		if (lumThreshold > 0) {

			threshold(lum, lum, lumThreshold, 255, 0);
			insect.updateHeight(lumThreshold);

			vector<Point2f> objectCentres = findObjects(lum);
			if (objectCentres.size() > 0) {

				insect.updatePosition(objectCentres[0], ROI);

#ifdef KALMAN
				//Prediction
				Mat predict = KF.predict();
				Point2f xy_loc(predict.at<float>(0), predict.at<float>(1));
				Point2f xy_vel(predict.at<float>(2), predict.at<float>(3));

				//Attempt to allow tracking of vanishing target
				KF.statePre.copyTo(KF.statePost);
				KF.errorCovPre.copyTo(KF.errorCovPost);

				//Get measurements
				if (useContours.size() >= 1) {
					measurement.at<float>(0) = mc[0].x + float(ROI.x);
					measurement.at<float>(1) = mc[0].y + float(ROI.y);
				}
				else {
					measurement.at<float>(0) = xy_loc.x;
					measurement.at<float>(1) = xy_loc.y;
				}


				//Update filter
				Mat correction = KF.correct(measurement);
				Point2f stateLoc(correction.at<float>(0), correction.at<float>(1));

				Point2f stateVel(correction.at<float>(2), correction.at<float>(3));
				Point2f measLoc(measurement.at<float>(0), measurement.at<float>(1));
				targetv.push_back(measLoc);
				kalmanv.push_back(stateLoc);
#ifdef DEBUG
				// plot stuff
				src = Scalar::all(0);


				drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
				drawCross(src, measLoc, Scalar(0, 0, 255), 5);

				for (int i = 0; i < targetv.size() - 1; i++)
					line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

				for (int i = 0; i < kalmanv.size() - 1; i++)
					line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

				imshow("Frame Kalman", src);
#endif
#endif //Kalman

				ROI = updateROI(ROI, insect.position, src);

#ifdef DEBUG
				Mat srcBox = src.clone();
				rectangle(srcBox, ROI, Scalar(255, 255, 255), 2, 8, 0);
				imshow("Source w Box", srcBox);
				imshow("Frame", src_ROI);
				imshow("Luminance", lum);
				printf("Height Bracket: %i	", insect.heightBracket);

				/// Draw contours
				Mat contourOutput = Mat::zeros(src_ROI.size(), CV_8UC3);
				for (int i = 0; i < objectCentres.size(); i++)
				{
					circle(contourOutput, objectCentres[i], 4, Scalar(255, 0, 0), -1, 8, 0);
				}
				imshow("Contours", contourOutput);
				printf("Speed: %.1f	", insect.speed);
#endif // DEBUG
			}
		}

		capture >> src;
		//resize(src, src, Size(), 0.3, 0.3);

		//wait_period = 50;
		waitKey(10); // fps won't be accurate unless this period is defined as wait_period (variable shared with fps counter).
		printf("\n");
	}

	capture.release();

	//while (waitKey(10) < 0) {
		cout << "Done\n";
	//}

	return(0);
}
#endif RUN