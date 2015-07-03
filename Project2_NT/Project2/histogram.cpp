#include "histogram.h"

MatND findHistogram(Mat inputImage, int debug_flag, int numBins) {
	MatND hist;
	MatND histNormal;


	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	if (debug_flag){

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

		namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
		imshow("calcHist Demo", histImage);
	}

	return hist;
}

vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize) {

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

void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth) {

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

int findThresholdByArea(vector<int> inputHistogram, int minArea)
{
	int threshSum = 0;
	int i = 0;
	for (i = inputHistogram.size() - 1; i > 0; i--) {
		threshSum += inputHistogram[i];

		if (threshSum > minArea)
		{
			break;
		}
	}
	return i;
}

int findThreshold(Mat inputImage, int previousThreshold, bool& noBug, int debug_flag) {

	MatND hist = findHistogram(inputImage, debug_flag);
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