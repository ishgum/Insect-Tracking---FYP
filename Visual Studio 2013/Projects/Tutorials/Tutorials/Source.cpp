#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;


RNG rng(12345);


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


#define DEBUG
#define FPS
#define UseWebcam
//#define ShowHistogram

struct myLine
{
	Point start;
	Point finish;
	Point difference;
	Point2f normalisedDifference;
	int length;
	float angle;
};



// Finds the histogram for the given single dimensional matrix
MatND findHistogram(Mat inputImage, int numBins = 128) {
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

	for (int i = 0; i < histSize; i++) {
		printf("%i = %f", i, hist.at<float>(i));
		printf("   %i = %f\n", i, histNormal.at<float>(i));
	}

	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);


#endif // !ShowHistogram

	return hist;
}



// Takes a moving average filter of the histogram and returns the histogram as a vector of integers
vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = 5) {

	vector<int> movingAverage;

	int averageArraySize = static_cast<int>(histogramInput.total()) - averageSize - 1;

	for (int i = 0; i < averageArraySize; i++) {
		float sum = 0;
		for (int j = 0; j < averageSize; j++) {
			//pos = i + j;
			sum += histogramInput.at<float>(i + j);
		}

		movingAverage.push_back(sum / averageSize);
	}
	return movingAverage;
}



// Finds all local peaks a minimums using a user-defined width
void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = 10) {
	
	int forLoopMin = -findWidth;
	int forLoopMax = findWidth;



	for (int i = 0; i < inputHistogram.size(); i++) {
		if (i < findWidth) {
			forLoopMin = -i;
		}
		else if ((i + findWidth) > inputHistogram.size()) {
			forLoopMax = inputHistogram.size() - i;
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
		if (currentMax == inputHistogram[i] && currentMax != 0 && currentMax > 500) {
			peaks.push_back(i);
		}
	}
}



// Application specific function which finds the right most peak, and then searches for the next minimum to set as the threshold value
int findThreshold(Mat inputImage) {

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
		printf("No line found\n");
		return NULL;
	}

	int secondPeak = peaks[peaks.size() - 2];
	if ((peakLoc - secondPeak) < 20) {
		peakLoc = secondPeak;
	}

	for (int i = mins.size()-1; i > 0; i--) {
		if (mins[i] < peakLoc) {
			printf("Threshold Location is: %i\n", mins[i]);
			return mins[i];
		}
	}


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



// Performs simple blob rejection based on area and approximates a polygon fit
vector<vector<Point>> processContours(vector<vector<Point> > contours, vector<Vec4i> hierarchy, Size matrixSize, int maxError = 10, int minContourArea = 5000) {

	
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


	int j = 0;
	vector<vector<Point> > useContours(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		if ((mu[i].m00 > minContourArea)) {
			useContours[j] = vector<Point>(contours[i]);
			approxPolyDP(Mat(useContours[j]), useContours[j], maxError, true);
			j++;
		}
	}

	vector<vector<Point>> finalContours(j);
	for (int k = 0; k < j; k++) {
		finalContours[k] = useContours[k];
	}

	return finalContours;
}



// Calculates relevant properties of a line based on a start and finish point
myLine calculateLineProperties(myLine line) {
	line.difference = line.finish - line.start;
	line.length = sqrt((line.difference.x)*(line.difference.x) + (line.difference.y)*(line.difference.y));
	line.angle = atan(line.difference.y / (line.difference.x + 0.0000001)) * 180 / CV_PI;
	line.normalisedDifference = Point2f((float(line.difference.x)) / line.length, float((line.difference.y)) / line.length);

	return line;
}



// Takes a series of points and creates a series of lines
vector <vector <myLine>> converPointsToLines(vector <vector<Point>> inputContour) {
	int numD = 0;
	vector <vector<myLine>> allLines(inputContour.size());
	for (int nC = 0; nC < inputContour.size(); nC++) {
		numD = 0;
		vector <myLine> lines(inputContour[nC].size());

		for (int nP = 0; nP < inputContour[nC].size(); nP++) {
			if (nP > 0) {
				lines[numD].start = inputContour[nC][nP];
				lines[numD].finish = inputContour[nC][nP - 1];
			}
			else {
				lines[numD].start = inputContour[nC][nP];
				lines[numD].finish = inputContour[nC][inputContour[nC].size() - 1];
			}

			lines[numD] = calculateLineProperties(lines[numD]);
			numD++;
		}
		allLines[nC] = lines;
	}
	return allLines;
}



// Combines consecutive lines which are of a similar angle
vector <vector <myLine>> reduceLinesOfSimilarAngle(vector <vector <myLine>> lines, int tolerance = 10) {

	myLine tempLine;
	myLine nextLine;
	bool shrunk = 1;

	while (shrunk != 0) {
		shrunk = 0;
		for (int i = 0; i < lines.size(); i++) {
			for (int j = 0; j < lines[i].size(); j++) {
				if (j < (lines[i].size() - 1)) {
					tempLine = lines[i][j];
					nextLine = lines[i][j + 1];
				}
				else {
					tempLine = lines[i][j];
					nextLine = lines[i][0];
				}

				if (abs(abs(tempLine.angle) - abs(nextLine.angle)) < 10) {
					tempLine.start = nextLine.start;
					tempLine = calculateLineProperties(tempLine);
					shrunk = 1;

					lines[i][j] = tempLine;

					if (j < (lines[i].size() - 1)) {
						lines[i].erase(lines[i].begin() + j + 1);
					}
					else {
						lines[i].erase(lines[i].begin());
					}
				}
			}
		}
	}
	return lines;
}



// Simple line rejection based on line length
vector <vector <myLine>> removeShortLines(vector <vector< myLine>> inputLines, int minimumLength = 100) {
	vector <vector< myLine>> outputLines(inputLines.size());

	for (int i = 0; i < inputLines.size(); i++) {
		for (int j = 0; j < inputLines[i].size(); j++) {
			if (inputLines[i][j].length > minimumLength) {
				outputLines[i].push_back(inputLines[i][j]);
			}
		}
	}
	return outputLines;
}



// Maps parallel lines to their angles
map<float, vector<myLine>> groupParallelLines(vector <vector <myLine>> inputLines, int angleTolerance = 2) {
	map<float, vector<myLine>> useLineMap;

	for (int i = 0; i < inputLines.size(); i++) {
		for (int j = 0; j < inputLines[i].size(); j++) {
			for (int k = 0; k < inputLines[i].size(); k++) {
				if (abs(inputLines[i][j].angle - inputLines[i][k].angle) < angleTolerance && j != k) {
					int averageAngle = (inputLines[i][j].angle + inputLines[i][k].angle) / 2;
					useLineMap[averageAngle].push_back(inputLines[i][j]);
				}
			}
		}
	}
	return useLineMap;
}



// Finds the centre of two parallel lines
vector <myLine> findCentre(map<float, vector<myLine>> inputLineMap) {
	
	vector <myLine> extractedLines;
	myLine tempLine2;
	Point averageStart;
	Point averageFinish;

	while (!inputLineMap.empty()) {

		vector<myLine> lines;
		lines = inputLineMap.begin()->second;

		if (lines.size() == 2) {
			if (norm(lines[0].start - lines[1].start) < norm(lines[0].start - lines[1].finish)) {
				averageStart = Point((lines[0].start.x + lines[1].start.x) / 2, (lines[0].start.y + lines[1].start.y) / 2);
				averageFinish = Point((lines[0].finish.x + lines[1].finish.x) / 2, (lines[0].finish.y + lines[1].finish.y) / 2);
			}
			else {
				averageStart = Point((lines[0].start.x + lines[1].finish.x) / 2, (lines[0].start.y + lines[1].finish.y) / 2);
				averageFinish = Point((lines[0].finish.x + lines[1].start.x) / 2, (lines[0].finish.y + lines[1].start.y) / 2);
			}
			tempLine2.start = averageStart;
			tempLine2.finish = averageFinish;
			tempLine2 = calculateLineProperties(tempLine2);
			extractedLines.push_back(tempLine2);
		}

		inputLineMap.erase(inputLineMap.begin());
	}
	return extractedLines;
}



// Encapsulating function to find a centre line from a contour
vector <myLine> processLines(vector <vector<Point>> inputContours) {
	vector <vector<myLine>> allLines;
	map<float, vector<myLine>> groupedParallelLines;
	vector <myLine> extractedLines;

	allLines = converPointsToLines(inputContours);
	allLines = reduceLinesOfSimilarAngle(allLines);
	allLines = removeShortLines(allLines);
	groupedParallelLines = groupParallelLines(allLines);
	extractedLines = findCentre(groupedParallelLines);

	return extractedLines;
}


void drawLine(Mat image, float theta, float rho) {
	Point pt1, pt2;
	double a = cos(theta), b = sin(theta);
	double x0 = a*rho, y0 = b*rho;
	pt1.x = cvRound(x0 + 1000 * (-b));
	pt1.y = cvRound(y0 + 1000 * (a));
	pt2.x = cvRound(x0 - 1000 * (-b));
	pt2.y = cvRound(y0 - 1000 * (a));
	line(image, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
}

Mat findLines(Mat inputImage, int minAngleDifference = 60, int minLineLength = 80) {


	Mat cannyContours;	Mat initialLines;	Mat finalLines;
	map<float, vector<float>> lineMap;
	vector<Vec2f> extractedLines;

	Canny(inputImage, cannyContours, 50, 200, 3);
	cvtColor(cannyContours, initialLines, CV_GRAY2BGR);
	cvtColor(cannyContours, finalLines, CV_GRAY2BGR);

	HoughLines(cannyContours, extractedLines, 1, CV_PI / minAngleDifference, minLineLength, 0, 0);
	

	// Sort lines by angle to find parallel lines
	for (size_t i = 0; i < extractedLines.size(); i++)
	{
		float rho = extractedLines[i][0];
		float theta = extractedLines[i][1];
		lineMap[theta].push_back(rho);

#ifdef DEBUG
		drawLine(initialLines, rho, theta);
#endif // 
	}

#ifdef DEBUG
	imshow("Initial Lines", initialLines);
#endif // 


	while (!lineMap.empty()) {

		vector<float> lineRhos;
		lineRhos = lineMap.begin()->second;

		float minRho = lineRhos[0];
		float maxRho = lineRhos[0];

		if (lineRhos.size() > 1) {
			for (int i = 0; i < lineRhos.size(); i++) {
				if (lineRhos[i] < minRho) {
					minRho = lineRhos[i];
				}
				if (lineRhos[i] > maxRho) {
					maxRho = lineRhos[i];
				}
			}

			float theta = lineMap.begin()->first;
			float rho = (maxRho + minRho)/2;

			drawLine(finalLines, theta, rho);
		}

		cout << lineMap.begin()->first << " => " << lineMap.begin()->second.size() << '\n';
		lineMap.erase(lineMap.begin());
	}
	return finalLines;
}




/** @function main */
	int main(int argc, char** argv)
	{
		Mat src;	Mat src_hsv;
		Mat hsvValues[3];


#ifdef FPS
		int frame_num = 1;
		// Timing functions
		int wait_period = 50;				/// Waitkey period
		double wall0 = get_wall_time();
		double cpu0 = get_cpu_time();
		int num_frames_proc = -1;
		float fps_wall, fps_cpu = 0;
		double cpu_running_total = 0;

#endif // FPS
#ifdef UseWebcam

		CvCapture* capture = cvCreateCameraCapture(0);

		while (waitKey(10) < 0) {
			src = cvQueryFrame(capture);
#endif // UseWebcam

#ifndef UseWebcam
			src = imread("C:/Users/myadmin/Documents/Image/MichaelsTestImages/Test Images/TestImage11.jpg");
#endif // !UseWebcam



			Mat srcBox = src.clone();

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
			imshow("FPS", src_w_text);
			if ((frame_num % 100) == 0){ //show only every 100th frame
				//	imshow("FPS", src_w_text);
			}
			printf("\nFrame: %i\tCPU total: %f\tCPU FPS: %f", frame_num, cpu_running_total, fps_cpu);
			frame_num++;
#endif // FPS	


			Mat dst;
			// Eliminate noise with a Gaussian blur
			GaussianBlur(src, dst, Size(1, 1), 0, 0);
			//bilateralFilter(dst, src, 5, 300, 300);

			/*
			// Convert image to HSV
			cvtColor(src, src_hsv, CV_BGR2HSV);
			split(src_hsv, hsvValues);

			// Create matrices for saturation and value. (Not interested in hue)
			Mat sat = hsvValues[1];
			Mat value = hsvValues[2];
			*/
			Mat src_hsl; Mat hslValues[3];


			cvtColor(src, src_hsl, CV_BGR2HLS);
			split(src_hsl, hslValues);

			Mat sat = hslValues[2];
			Mat value = hslValues[1];





#ifdef DEBUG
			imshow("Source", src);
			imshow("Sat1", sat);
			imshow("Value1", value);
#endif // DEBUG


			//Saturation Thresholding   -    Saturation thresholding is the main focus of the background segmentation

			medianBlur(sat, sat, 11);

			int saturationThreshold = findThreshold(sat);		// Perform Dynamic thresholding on the saturation image
			//if (saturationThreshold != NULL) {					// If the background is able to be segmented, continue.
			threshold(sat, sat, saturationThreshold * 2, 255, 0);



			// Value Thresholding   -   A non-agressive value thresholding is performed to eliminate any false-lines caused by black areas on the screen

			threshold(value, value, 50, 255, 0);
			GaussianBlur(value, value, Size(11, 11), 0, 0);		//Simple dilate operation
			threshold(value, value, 200, 255, 0);


			// Sum images to extract only the desired line
			Mat extractedLine = sat & value;



#ifdef DEBUG
			imshow("Sat2", sat);
			imshow("Value", value);
			namedWindow("Sum", CV_WINDOW_AUTOSIZE);
			imshow("Sum", extractedLine);
#endif // DEBUG
			waitKey(10);
			vector<vector<Point> > contours;
			vector<vector<Point>> useContours;
			vector<Vec4i> hierarchy;
			vector <myLine> extractedLines;

			findContours(extractedLine, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			useContours = processContours(contours, hierarchy, extractedLine.size());		// Eliminates contours of too large and area and approximates the shape to reduce number of points
			extractedLines = processLines(useContours);				// Uses the contour image to find the centre line for the tape


#ifdef DEBUG
			Mat contourImage = Mat::zeros(extractedLine.size(), CV_8UC3);
			for (int i = 0; i < useContours.size(); i++)
			{
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
				drawContours(contourImage, useContours, i, color, CV_FILLED, 8, hierarchy, 0, Point());
			}

			imshow("Contour Image", contourImage);
			Mat superImposedSrc = contourImage + src;
			imshow("Superimposed Source", superImposedSrc);


			//Mat finalLines = findLines(contourImage);
			//imshow("Drawing Lines", finalLines);

#endif // DEBUG

			// Draws the result

			Mat drawingLines(src);
			for (int j = 0; j < extractedLines.size(); j++) {
				line(drawingLines, extractedLines[j].start, extractedLines[j].finish, Scalar(255, 255, 0), 3, CV_AA);
			}

			imshow("Drawing Lines", drawingLines);
			//}

#ifdef UseWebcam
		}
		cvReleaseCapture(&capture);
#endif // UseWebcam

#ifndef UseWebcam
		waitKey(0);
#endif // !UseWebcam
		return(0);

	}


