#include "Threshold.h"



// Finds the histogram for the given single dimensional matrix
MatND findHistogram(Mat inputImage, int numBins = NUM_BINS) {
	MatND hist;
	MatND histNormal;

	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, numBins };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);


#ifdef SHOW_HISTOGRAM	//Prints the histogram

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
//		printf("%i = %f", i, hist.at<float>(i));
//		printf("   %i = %f\n", i, histNormal.at<float>(i));
//	}

	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);


#endif // !ShowHistogram

	return hist;
}



// Takes a moving average filter of the histogram and returns the histogram as a vector of integers. Converts the histogram to a vector.
vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = AVERAGE_SIZE) {

	vector<int> movingAverage;

	int averageArraySize = static_cast<int>(histogramInput.total()) - averageSize - 1;

	for (int i = 0; i < averageArraySize; i++) {
		float sum = 0;
		for (int j = 0; j < averageSize; j++) {
			sum += histogramInput.at<float>(i + j);
		}

		movingAverage.push_back(sum / averageSize);
	}
	return movingAverage;
}



// Finds all local peaks a minimums using a user-defined width. Find width changes the definition of "local"
void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = FIND_WIDTH) {

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
		if (currentMax == inputHistogram[i] && currentMax > 20) {
			peaks.push_back(i);
		}
	}
}


//Finds and maps all of the lobes in the histogram. Has a default minimum area of 10 pixels.
map<int, int> findLobes(vector<int> inputHistogram, vector<int>& mins, int minLobeArea = MIN_LOBE_AREA) {
	map<int, int> lobeMap;
	for (int i = 0; i < mins.size() - 2; i++) {
		int currentSum = 0;
		for (int j = mins[i]; j < mins[i + 1]; j++) {
			currentSum += inputHistogram[j];
		}
		if (currentSum > minLobeArea)	lobeMap[mins[i]] = currentSum;
	}
	return lobeMap;
}


// Application specific function which finds the right most peak, and then searches for the next minimum to set as the threshold value
int findThreshold(Mat inputImage) {

	MatND hist = findHistogram(inputImage);
	vector<int> histMovingAverage = movingAverageFilterHistogram(hist);

	vector<int> peaks;
	vector<int> mins;

	findPeaksandMins(histMovingAverage, peaks, mins);
	map<int, int> lobeMap = findLobes(histMovingAverage, mins);

	

	if (lobeMap.size() > 1) {
		return lobeMap.begin()->first;
	}
	else
	{
		return -1;
	}
}

