
#define RUN
#ifdef RUN


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <ctype.h>

using namespace cv;
using namespace std;



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


#define ROI_SIZE .15
//#define DEBUG		//display video output windows
//#define FPS //wall breaks on release mode
#define KALMAN
#define ShowHistogram
#define HEIGHT_OFFSET 10
//#define RECORD_SOURCE_W_BOX		//record source footage with ROI overlay
#define FIND_DEPTH


RNG rng(12345);

#define THRESH_FILTER_SIZE 10

bool noBug = false;
int threshCount = 0;
int threshFilter[THRESH_FILTER_SIZE] = { 0 };


MatND findHistogram(Mat inputImage, int numBins = 256) {
	MatND hist;
	MatND histNormal;


	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

#ifdef ShowHistogram

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, histNormal, 0, histImage.rows, NORM_MINMAX, -1, Mat());


	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histNormal.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNormal.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

	//for (int i = 0; i < histSize; i++) {
	//	printf("%i = %f", i, hist.at<float>(i));
	//	printf("   %i = %f\n", i, histNormal.at<float>(i));
	//}
#ifdef DEBUG
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);
#endif // DEBUG


#endif // !ShowHistogram

	return hist;
}

vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = 1) {

	vector<int> movingAverage;

	int averageArraySize = static_cast<int>(histogramInput.total()) - averageSize + 1;

	for (int i = 0; i < averageArraySize; i++) {
		float sum = 0;
		for (int j = 0; j < averageSize; j++) {
			sum += histogramInput.at<float>(i + j);
		}

		movingAverage.push_back(sum / averageSize);
	}
	return movingAverage;
}

void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = 15) {

	int forLoopMin = -findWidth;
	int forLoopMax = findWidth;



	for (int i = 0; i < inputHistogram.size(); i++) {
		if (i < findWidth) {
			forLoopMin = -i;
			forLoopMax = findWidth;
		}
		else if ((i + findWidth) > inputHistogram.size()) {
			forLoopMax = inputHistogram.size() - i;
			forLoopMin = -findWidth;
		}
		else {
			forLoopMin = -findWidth;
			forLoopMax = findWidth;
		}

		int currentMin = inputHistogram[i];
		int currentMax = inputHistogram[i];

		for (int k = forLoopMin; k < forLoopMax; k++) {
			if (inputHistogram[i + k] < currentMin) {
				currentMin = inputHistogram[i + k];
			}
			if (inputHistogram[i + k] > currentMax) {
				currentMax = inputHistogram[i + k];
			}
		}
		if (currentMin == inputHistogram[i]) {
			mins.push_back(i);
		}
		if (currentMax == inputHistogram[i] && currentMax != 0) {
			peaks.push_back(i);
		}
	}
}

int findThresholdByArea(vector<int> inputHistogram, int minArea = 30)
{
	int threshSum = 0;
	int i = 0;
	for (i = inputHistogram.size()-1; i > 0; i--) {
		threshSum += inputHistogram[i];

		if (threshSum > minArea)
		{
			break;
		}
	}
	return i;
}



int findThreshold(Mat inputImage, int previousThreshold) {

	MatND hist = findHistogram(inputImage);
	vector<int> histMovingAverage = movingAverageFilterHistogram(hist);
	int areaLoc = findThresholdByArea(histMovingAverage);

	vector<int> peaks;
	vector<int> mins;

	findPeaksandMins(histMovingAverage, peaks, mins);

	int maxPeak = 0;
	int maxPeakLoc = 0;
	for (int i = 0; i < peaks.size(); i++) {
		if (histMovingAverage[peaks[i]] > maxPeak)
		{
			//printf("Max Location is: %i		", peaks[i]);
			maxPeak = histMovingAverage[peaks[i]];
			maxPeakLoc = peaks[i];
		}
	}

	int peakLoc = peaks[peaks.size() - 1];
	if ((areaLoc - maxPeakLoc) < 10 || peakLoc < maxPeakLoc) {
		//printf("Threshold Location is: %i- No bug found\n", previousThreshold);
		noBug = true;
		return previousThreshold;
	}


	for (int i = mins.size() - 1; i >= 0; i--) {
		if (mins[i] < areaLoc && mins[i] > maxPeakLoc) {
			//printf("Threshold Location is: %i\n", mins[i]);
			//printf("Width : %i	", (peakLoc - mins[i]));
			noBug = false;
			return mins[i];
		}
	}
	return previousThreshold;


	// Find the brightest X percentage of pixels in the frame
	//#define PercentageHistogram
#ifdef PercentageHistogram
	int histCount = 0;
	float histPcentThresh = percentage;
	double histSum = sum(hist)[0];
	float thresh = histSum * (histPcentThresh / 100.0);

	for (int i = histSize - 1; i >= 0; i--) {
		histCount += hist.at<float>(i);
		if (histCount >= thresh) {
			return i;
		}
	}
#endif // !PercentageHistogram
}

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

Mat preprocessImage(Mat image) {
	Mat values[3]; Mat image_hsl; Mat dst;
	static int lumThreshold = 0;


	// Conversion uses significant processor time,
	// using point grey camera we should be able to skip this step,
	// as it proivdes only one 'brightness' channel.
	cvtColor(image, image_hsl, CV_BGR2HLS);		// Convert image to HSL
	split(image_hsl, values);						// Split into channels

	Mat lum = values[1];
	medianBlur(lum, dst, 1);
	lumThreshold = findThreshold(dst, lumThreshold);		//Perform Dynamic thresholding on the saturation image
	threshFilter[threshCount++] = lumThreshold;
	
	MatND hist;
	MatND histNormal;
	/// Establish the number of bins
	int histSize = 256;
	float range[] = { lumThreshold, 256 };
	const float* histRange = { range };

	calcHist(&dst, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, histNormal, 0, histImage2.rows, NORM_MINMAX, -1, Mat());


	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage2, Point(bin_w*(i - 1), hist_h - cvRound(histNormal.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNormal.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

#ifdef DEBUG
	namedWindow("calcHist Demo2", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo2", histImage2);
#endif
	threshold(dst, dst, lumThreshold, 255, 0);



#ifdef DEBUG
	imshow("Luminance", dst);
#endif // DEBUG


	GaussianBlur(dst, dst, Size(11, 11), 2);
	threshold(dst, dst, 5, 255, 0);

	return dst;
}

void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

Rect updateROI(Rect ROI, Point stateLoc, Mat src) {
	int roiSize = ROI_SIZE * src.rows;


	if (noBug == false) {

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
	}
	return ROI;
}




/** @function main */
int main(int argc, char** argv)
{
	const string videoFile[] = {
		"C:/Users/myadmin/Videos/plainLow1.avi"
		"C:/Users/myadmin/Videos/plainLow2.avi"
		"C:/Users/myadmin/Videos/plainLow3.avi"
		"C:/Users/myadmin/Videos/plainMed1.avi"
		"C:/Users/myadmin/Videos/plainMed2.avi"
		"C:/Users/myadmin/Videos/plainMed3.avi"
		"C:/Users/myadmin/Videos/plainHigh1.avi"
		"C:/Users/myadmin/Videos/plainHigh2.avi"
		"C:/Users/myadmin/Videos/plainHigh3.avi"
		"C:/Users/myadmin/Videos/retroHigh1.avi"
		"C:/Users/myadmin/Videos/retroFar1.avi"
	};

	VideoCapture capture;

	//EARLY TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Ancient_times/plainHigh1.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2990.MOV"); //runs at ~6fps
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2987.MOV");

	//IR RREFLEC TESTS:
	//capture.open("C:/Users/myadmin/Documents/IR footage/retro2_2015-05-09-193310-0000.avi");
//	capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000.avi");
	capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only.avi"); 

	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193006-0000_8bit_uncompressed.avi"); // Princess Beetle and the sparkly dress, Co-Staring Michael
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro1_2015-05-09-192708-0000.avi"); //persistent bright region on lower portion of frame

	//DEPTH TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun2_0.avi");

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

#ifdef FPS
	int frame_num = 1;
	// Timing functions
	int wait_period = 10;				/// Waitkey period
	double wall0 = get_wall_time();
	double cpu0 = get_cpu_time();
	int num_frames_proc = -1;
	float fps_wall, fps_cpu = 0;
	double cpu_running_total = 0;

#endif // FPS


	int kalmanCount = 0;
	Mat src; Mat src_ROI;
	Point prevCentre;

	capture >> src;
	//resize(src, src, Size(), 0.3, 0.3);
	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame


	while (!src.empty()) {


		Mat srcBox = src.clone();
		rectangle(srcBox, ROI, Scalar(255, 255, 255), 2, 8, 0);
#ifdef DEBUG
		imshow("Source w Box", srcBox);
#endif
#if defined(RECORD_SOURCE_W_BOX) && ! defined(FPS)
		outputVideo.write(srcBox); //write output video w/o text
#endif

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
			//printf("\tFPS:  %f\t", fps);
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
		//resize(srcBox, srcBox, Size(), 0.3, 0.3);
#ifdef DEBUG
		imshow("Frame", src_ROI);
#endif // DEBUG

		//imshow("Frame", src_ROI);

		Mat dst = preprocessImage(src_ROI);

#define FIND_DEPTH
		int threshSum = 0;
		for (int p = 0; p < THRESH_FILTER_SIZE; p++) {
			threshSum += threshFilter[p];
		}
		printf("Threshold: %i	", threshSum / THRESH_FILTER_SIZE);
		printf("Height Bracket: %i	", threshSum / (THRESH_FILTER_SIZE * 20));
		if (threshCount == THRESH_FILTER_SIZE) 
			{ threshCount = 0; }
#endif
		/*****		CONTOURS		****/
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		Canny(dst, dst, 100, 100 * 2, 3);


		findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));


		/// Get the moments
		vector<Moments> mu(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
		}

		///  Get the mass centers:
		vector<Point2f> mc(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}


		//Filter by area
		int j = 0;
		vector<vector<Point> > useContours(contours.size());
		for (int i = 0; i < contours.size(); i++) {
			if ((mu[i].m00 < 20000) && (mu[i].m00 > 5)) {
				useContours[j] = vector<Point>(contours[i]);
				j++;
				//printf("Area %i: %.1f ", j, mu[i].m00);
			}
		}

		if (j == 0) {
			noBug = true;
		}
		else {
			noBug = false;
		}


		/// Draw contours
		Mat drawing = Mat::zeros(dst.size(), CV_8UC3);
		//printf("\nCounter Sizes: ");
		for (int i = 0; i < useContours.size(); i++)
		{
			Scalar color = Scalar(100*i, 100*i, 255);
			drawContours(drawing, useContours, i, color, 1, 8, hierarchy, 0, Point());
			//circle(drawing, mc[i], 4, color, -1, 8, 0);
		}

		/// Show in a window
#ifdef DEBUG
		namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		imshow("Contours", drawing);
#endif // DEBUG



#ifdef KALMAN
		//Prediction
		Mat predict = KF.predict();
		Point xy_loc(predict.at<float>(0), predict.at<float>(1));
		Point xy_vel(predict.at<float>(2), predict.at<float>(3));

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
		Point stateLoc(correction.at<float>(0), correction.at<float>(1));
	
		Point stateVel(correction.at<float>(2), correction.at<float>(3));
		Point measLoc(measurement.at<float>(0), measurement.at<float>(1));

		// plot stuff
		src = Scalar::all(0);

		targetv.push_back(measLoc);
		kalmanv.push_back(stateLoc);
		drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
		drawCross(src, measLoc, Scalar(0, 0, 255), 5);

		for (int i = 0; i < targetv.size() - 1; i++)
			line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

		for (int i = 0; i < kalmanv.size() - 1; i++)
			line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);
#ifdef DEBUG
		imshow("Frame Kalman", src);
#endif
#endif //Kalman
		

		Point contourCentre;
		
		Point centreDiff;

		if (noBug == true) {
			kalmanCount = 0;
		}
		else {
			contourCentre = Point(mc[0].x + ROI.x, mc[0].y + ROI.y);
		}

		centreDiff = contourCentre - prevCentre;
		float value = norm(centreDiff);
		printf("Speed: %.1f	", value);
		//printf("Just trying: %i", (value + threshSum / THRESH_FILTER_SIZE));
		prevCentre = contourCentre;


		ROI.x = 0;
		ROI.y = HEIGHT_OFFSET;		//Strange artifacts in top left hand corner removed
		ROI.width = src.cols;
		ROI.height = src.rows-HEIGHT_OFFSET;

		if (kalmanCount++ > 10) {

			ROI = updateROI(ROI, contourCentre, src);
		}

		capture >> src;
		//resize(src, src, Size(), 0.3, 0.3);

#ifndef FPS
		int wait_period = 10;
#endif
		//wait_period = 50;
		waitKey(wait_period); // fps won't be accurate unless this period is defined as wait_period (variable shared with fps counter).
		printf("\n");
	}

	capture.release();

	//while (waitKey(10) < 0) {
		cout << "Done\n";
	//}

	return(0);
}



/*
Mat image;
image = imread("C:/Users/myadmin/Documents/Image/TestImageInfraRed.jpg", 1);


Mat gray_image;
cvtColor(image, gray_image, CV_BGR2GRAY);

Mat small_image(image, Range::all(), Range(1, 500));

namedWindow("Pointless", CV_WINDOW_AUTOSIZE);
namedWindow("Gray image", CV_WINDOW_AUTOSIZE);
namedWindow("Small image", CV_WINDOW_AUTOSIZE);


imshow("Pointless", image);
imshow("Gray image", gray_image);
imshow("Small image", small_image);
Z*/


/*

int alpha = 0; // Simple contrast control
int beta = 1;

void contrast(void) {
namedWindow("Contrast", CV_WINDOW_AUTOSIZE);

createTrackbar("Alpha:", "Contrast", &alpha, 3);
createTrackbar("Beta:", "Contrast", &beta, 100);
/// Load source image and convert it to gray

Mat src_2 = Mat::zeros(src.size(), src.type());

for (int y = 0; y < src.rows; y++)
{
for (int x = 0; x < src.cols; x++)
{
for (int c = 0; c < 3; c++)
{
src_2.at<Vec3b>(y, x)[c] =
saturate_cast<uchar>(alpha*(src.at<Vec3b>(y, x)[c]) + beta);
}
}
}

imshow("Contrast", src_2);
}
*/




