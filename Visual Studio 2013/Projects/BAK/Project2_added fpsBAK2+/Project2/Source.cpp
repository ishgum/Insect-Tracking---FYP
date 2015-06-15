
#define RUN
#ifdef RUN


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\video\tracking.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <ctype.h>
#include <sys/timeb.h>


using namespace cv;
using namespace std;

Mat src; Mat src_ROI; Mat src_hsl; 	Mat dst; Mat morph; Mat frame;

RNG rng(12345);

#define OVERALL_SCALE_X 0.3  // Simple global downsampling for performance, fraction /1
#define OVERALL_SCALE_Y 0.3
#define ROI_SIZE_X 300  // Size of first finding attempt region, #pixels
#define ROI_SIZE_Y 200

int d = 0;
int col = 0;
int space = 0;

int edgeThresh = 1;
int lowThreshold = 80;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;


int morph_elem = 0;
int morph_size = 0;
int morph_operator = 0;
int const max_operator = 4;
int const max_elem = 2;
int const max_kernel_size = 21;

int printVector = 0;

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
#endif



MatND findHistogram(Mat inputImage, int numBins = 128) {
	MatND hist;
	MatND histNormal;


	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	#define ShowHistogram
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

	namedWindow("3.0 calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("3.0 calcHist Demo", histImage);


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

int findThreshold(Mat inputImage, int previousThreshold) {

	MatND hist = findHistogram(inputImage);
	vector<int> histMovingAverage = movingAverageFilterHistogram(hist);

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
	if ((peakLoc - maxPeakLoc) < 10 || peakLoc < maxPeakLoc) {
		//printf("Threshold Location is: %i- No bug found\n", previousThreshold);
		return previousThreshold;
	}


	for (int i = mins.size() - 1; i >= 0; i--) {
		if (mins[i] < peakLoc && mins[i] > maxPeakLoc) {
			//printf("Threshold Location is: %i\n", mins[i]);
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





/// Function header
void filter(int, void*);

void Morphology_Operations(int, void*);


void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

void display_fps(){

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
	//while (1) {

	//capture.open("C:/Users/myadmin/Videos/plainHigh1.avi");
	capture.open("C:/Users/myadmin/Documents/Image/Tests/MVI_2989.MOV");


	KalmanFilter KF(4, 2, 0);
	Mat state(4, 1, CV_32F); //x, y, delta x, delta y
	Mat processNoise(4, 1, CV_32F);
	Mat measurement = Mat::zeros(2, 1, CV_32F);

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

	vector<Point> targetv, kalmanv;

	int lumThreshold = 0;
	int frame_num = 1;

	Mat patch;

	capture >> src;
	printf("Capture Dim: %i by %i\n", src.cols, src.rows);
	resize(src, src, Size(), OVERALL_SCALE_X, OVERALL_SCALE_Y);

	// Timing functions
	int wait_period = 50;				/// Waitkey period
	double wall0 = get_wall_time();
	double cpu0 = get_cpu_time();
	int num_frames_proc = -1;
	float fps_wall, fps_cpu = 0;

	// INIT patch stuff
	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame
	Point patch_centre;
	int ROIsizex = ROI_SIZE_X; // left as ints to redfeine later
	int ROIsizey = ROI_SIZE_Y;
	int padleft, padright ,padtop ,padbottom;
	ROI.x = 0;
	ROI.y = 0;
	ROI.width = src.cols;
	ROI.height = src.rows;

	while (!src.empty()) {
		//src = src(ROI);
		
		// ****** Timing functions to display FPS
		num_frames_proc++;
		double wall1 = get_wall_time();
		double cpu1 = get_cpu_time();
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
		// add fps text to "1. Frame" window
		// Displayed FPS accounts for the delay we add in waitkey, far, far below.
		//wall time
		char fps_wall_c[4];
		sprintf(fps_wall_c, "wall FPS %.2f", fps_wall);
		Point fps_wall_text_loc(10, 30);
		putText(src_w_text, fps_wall_c, fps_wall_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, {255, 255, 255},1.5);
		
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
		////display source w/ fps
		imshow("1. Frame", src_w_text);
		// ******** End time

		//resize(src, src, Size(), 0.3, 0.3);
		//imshow("1. Frame", src);


		//resize(src, src, Size(), 0.3, 0.3);
		namedWindow("2. Raw video", CV_WINDOW_AUTOSIZE);
		imshow("2. Raw video", src);

		/* Search ROI first*/
		//success = 0;
		//while (!success)


		//src_ROI = src(ROI);
		src_ROI = src;

		Mat values[3];

		// Convert image to HSL
		cvtColor(src_ROI, src_hsl, CV_BGR2HLS);

		// Split into channels
		split(src_hsl, values);

		Mat lum = values[1];

		medianBlur(lum, dst, 1);
		//bilateralFilter(lum, dst, 10, 20, 40);


//#define OLD_HIS
#ifdef OLD_HIS
		//256 rows, 1 col
		MatND lum_hist;

		/// Establish the number of bins
		int histSize = 256;

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 };
		const float* histRange = { range };

		calcHist(&dst, 1, 0, Mat(), lum_hist, 1, &histSize, &histRange, true, false);


		// Draw the histograms for B, G and R
		//int hist_w = 512; int hist_h = 400;
		//int bin_w = cvRound((double)hist_w / histSize);

		//Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		/// Normalize the result to [ 0, histImage.rows ]
		//normalize(lum_hist, lum_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

		/*
		for (int i = 1; i < histSize; i++)
		{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(lum_hist.at<float>(i - 1))),
		Point(bin_w*(i), hist_h - cvRound(lum_hist.at<float>(i))),
		Scalar(255, 0, 0), 2, 8, 0);
		}
		*/

		/// Display
		//namedWindow("Luminance histogram", CV_WINDOW_AUTOSIZE);
		//imshow("Luminance histogram", histImage);

		//waitKey(0);



		/*
		int histCount = 0;
		int histThresh = 10;
		float histPcentThresh = 0.0001;
		int i = 0;
		double histSum = sum(lum_hist)[0];
		float lum_thresh = histSum * (histPcentThresh / 100.0);

		for (i = 255; i >= 0; i--) {
			histCount += lum_hist.at<float>(i);
			//histCount += lum_hist.ptr<uchar>(i)[1];

			/*if (histCount >= lum_thresh) {
			break;
			}*/

			if (histCount >= histThresh) {
				break;
			}
		}

endif
	
		cout << i;
		cout << endl;


		threshold(dst, dst, i - 1, 255, 0);
		//threshold(dst, dst, 100, 255, 0);
		//adaptiveThreshold(dst, dst, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, 1, 25, 6);
#endif // OLD_HIS

		
		lumThreshold = findThreshold(dst, lumThreshold);		//Perform Dynamic thresholding on the saturation image
		threshold(dst, dst, lumThreshold * 2, 255, 0);

		imshow("3. Luminance", dst);
		
		int morph_size = 4;
		GaussianBlur(dst, dst, Size(11, 11), 2);

		threshold(dst, dst, 5, 255, 0);

		//Mat element = getStructuringElement(2, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		/// Apply the specified morphology operation
		//morphologyEx(dst, dst, 1, element);

		//namedWindow("Blurred", CV_WINDOW_AUTOSIZE);
		//imshow("Blurred", src_hsl);


		//cvtColor(src_hsl, dst, CV_HLS2BGR);

		namedWindow("4. Source", CV_WINDOW_AUTOSIZE);
		imshow("4. Source", dst);


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
			if ((mu[i].m00 < 300) && (mu[i].m00 > 50)) {
				useContours[j] = vector<Point>(contours[i]);
				j++;
				printf("Found centres: %f, ", mu[i].m00);
			}
		}


		/// Draw contours
		Mat drawing = Mat::zeros(dst.size(), CV_8UC3);
		printf("\nCounter Sizes: ");
		for (int i = 0; i < useContours.size(); i++)
		{
			Scalar color = Scalar(0, 255, 255);
			drawContours(drawing, useContours, i, color, CV_FILLED, 8, hierarchy, 0, Point());
			//circle(drawing, mc[i], 4, color, -1, 8, 0);
		}



		/// Show in a window
		namedWindow("5. Contours", CV_WINDOW_AUTOSIZE);
		imshow("5. Contours", drawing);
		
		
		//Prediction
		Mat predict = KF.predict();
		Point xy_loc(predict.at<float>(0), predict.at<float>(1));
		Point xy_vel(predict.at<float>(2), predict.at<float>(3));

		//Attempt to allow tracking of vanishing target
		KF.statePre.copyTo(KF.statePost);
		KF.errorCovPre.copyTo(KF.errorCovPost);

		//Get measurements
		if (useContours.size() >= 1) {
			measurement.at<float>(0) = mc[0].x;
			measurement.at<float>(1) = mc[0].y;
		}
		else {
			measurement.at<float>(0) = xy_loc.x;
			measurement.at<float>(1) = xy_loc.y;
		}


		// Update filter
		Mat correction = KF.correct(measurement);
		Point stateLoc(correction.at<float>(0), correction.at<float>(1));
		Point stateVel(correction.at<float>(2), correction.at<float>(3));
		Point measLoc(measurement.at<float>(0), measurement.at<float>(1));

		/*END attempt at finding bug*/


		// plot stuff
		//src = Scalar::all(0); // uncomment to see path displayed on black background

		targetv.push_back(measLoc);
		kalmanv.push_back(stateLoc);
		drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
		drawCross(src, measLoc, Scalar(0, 0, 255), 5);

		for (int i = 0; i < targetv.size() - 1; i++)
			line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

		for (int i = 0; i < kalmanv.size() - 1; i++)
			line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

		imshow("6. Frame Kalman", src);
		printf("\nFrame Kalman is %i by %i", src.cols , src.rows);


		// Generate ROI for next Kalman iteration. For perf enhancement

		// Find bounds of rectangle
		patch_centre = stateLoc;
		//ROIsizex = 100;
		//ROIsizey = 100;
		padleft = patch_centre.x - ROIsizex / 2;
		padright = patch_centre.x + ROIsizex / 2;
		padtop = patch_centre.y - ROIsizey / 2;
		padbottom = patch_centre.y + ROIsizey / 2;

		// limit rectangle to bounds of source image.
		if (padleft < 0){
			patch_centre.x = ROIsizex / 2;
		}
		else if (padright > src.cols){
			patch_centre.x = src.cols - ROIsizex / 2;
		}

		if (padtop < 0){
			patch_centre.y = ROIsizey / 2;
		}
		else if (padbottom > src.rows){
			patch_centre.y = src.rows - ROIsizey / 2;
		}
		padleft = patch_centre.x - ROIsizex / 2;
		padright = patch_centre.x + ROIsizex / 2;
		padtop = patch_centre.y - ROIsizey / 2;
		padbottom = patch_centre.y + ROIsizey / 2;

		printf("\nPatch is centred at %i and %i", patch_centre.x, patch_centre.y);
		
		// Could use region directly or getRectSubPix, maybe there is a performance cost with this
		//getRectSubPix(src, Size(300, 300), stateLoc, patch);
		//patch = src;
		//patch = src(Rect(xy_loc.x, xy_loc.y, 100, 100));

		ROI.x = padleft;
		ROI.y = padtop;
		ROI.width = ROIsizex;
		ROI.height = ROIsizey;

		//patch = src(Rect(padleft, padtop, ROIsizex, ROIsizey));
		//patch = src(Rect(200, 150, 100, 100));
		patch = src(ROI);

		imshow("7. Patch", patch);
		printf("\nPatch is %i by %i\n", patch.cols, patch.rows);

		// Capture a new frame
		capture >> src;
		frame_num++;
		resize(src, src, Size(), OVERALL_SCALE_X, OVERALL_SCALE_Y);
		wait_period = 50; //change me so timer function can account for it. chee
		waitKey(wait_period);
	}

	capture.release();

	while (waitKey(10) < 0) {
		cout << "Done\n";
	}

	return(0);
}

#endif


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




