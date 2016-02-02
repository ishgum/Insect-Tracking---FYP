/****************************************************************************
 *
 *
 * @file threshold.cpp
 *
 * @brief Finds all of the lobes in a histogram image and returns the brightest lobe
 *
 * Functions for finding all of the lobes in a histogram image
 * 
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include "threshold.h"



// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Find Histogram
// ------------------------------------------------------------------------------

MatND findHistogram(Mat inputImage, int numBins = NUM_BINS) 
{
	MatND hist;

	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, numBins };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	return hist;
}



// ------------------------------------------------------------------------------
//   Show Histogram
// ------------------------------------------------------------------------------

void showHistogram (MatND inputHistogram, int histSize = NUM_BINS)
{
	MatND histNormal;

	// Sets the size of the histogram in the screen
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	// Creates a histogram image
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(inputHistogram, histNormal, 0, histImage.rows, NORM_MINMAX, -1, Mat());


	/// Draw each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histNormal.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNormal.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);
} 



// ------------------------------------------------------------------------------
//   Convert Histogram to Vector
// ------------------------------------------------------------------------------

vector<float> histToVector(MatND inputHistogram) 
{
	vector<float> histVector;

	for (int i = 0; i < static_cast<int>(inputHistogram.total()); i++)
	{
		histVector.push_back(inputHistogram.at<float>(i));
	}
	return histVector;
}


// ------------------------------------------------------------------------------
//   Moving Average Filter for histogram
// ------------------------------------------------------------------------------

vector<float> movingAverageFilter(vector<float> inputHistogram, int averageSize = AVERAGE_SIZE)
{
	vector<float> movingAverage;

	int averageArraySize = inputHistogram.size() - averageSize - 1;

	for (int i = 0; i < averageArraySize; i++) {
		float sum = 0;
		for (int j = 0; j < averageSize; j++) 
		{
			sum += inputHistogram[i + j];
		}

		movingAverage.push_back(sum / averageSize);
	}
	return movingAverage;
}


// ------------------------------------------------------------------------------
//   Find Peaks and Mins
// ------------------------------------------------------------------------------

void findPeaksandMins(vector<float> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = FIND_WIDTH) 
{
	int forLoopMin = -findWidth;
	int forLoopMax = findWidth;

	
	for (int i = 0; i < inputHistogram.size(); i++) 
	{
		// If the left extreme would go below zero, set it to zero
		if (i < findWidth) 
		{ 
			forLoopMin = -i; 
		}
		
		// If the right extreme would go above the histogram size, set to the max size 
		if ((i + findWidth) > inputHistogram.size()) 
		{
			forLoopMax = inputHistogram.size() - i;
		}

		// Set the local min and max to the current position
		bool localMin = true;
		bool localMax = true;
		float currentValue = inputHistogram[i];

		for (int k = forLoopMin; k < forLoopMax; k++) 
		{
			// If any of the values around it are greater or less than the current value then the current
			// value is not the local maximum or minimum respectively.
			if (inputHistogram[i + k] < currentValue) { localMin = false; }
			if (inputHistogram[i + k] > currentValue) { localMax = false; }
		}

		// If none of the values are less than the current, then add it to the mins vector
		if (localMin) {	mins.push_back(i); }
	
		// If none of the values are greater than the current and is above a minimum size, add to max vector
		if (localMax && currentValue > 20) { peaks.push_back(i); }
	}
}



// ------------------------------------------------------------------------------
//   Find Lobes
// ------------------------------------------------------------------------------

map<int, int> findLobes(vector<float> inputHistogram, vector<int>& mins, int minLobeArea = MIN_LOBE_AREA) 
{
	map<int, int> lobeMap;

	// Iterates through all of the minimums (these define the edges of the lobes)
	for (int i = 0; i < mins.size() - 2; i++) 
	{
		float currentSum = 0;

		// Sums all of the pixels between two consecutive minima
		for (int j = mins[i]; j < mins[i + 1]; j++) { currentSum += inputHistogram[j]; }

		// If the sum is greater than a specified minimum, add it to a lobe map. Key = sum, Value = associated left hand minimum
		if (currentSum > minLobeArea) { lobeMap[currentSum] = mins[i+1]; }
	}

	return lobeMap;
}



// ------------------------------------------------------------------------------
//   Find Threshold Macro
// ------------------------------------------------------------------------------

int findThreshold(Mat inputImage) {

	// Finds the histogram
	MatND hist = findHistogram(inputImage);
	vector<float> histVect = histToVector(hist);
	histVect = movingAverageFilter(histVect);

	// To find peaks and mins simultaneous, must be passed as pointers
	vector<int> peaks;
	vector<int> mins;
	findPeaksandMins(histVect, peaks, mins);

	map<int, int> lobeMap = findLobes(histVect, mins);

	// The lobe map is organised by keys, so the last entry should be the largest lobe. This returns
	// the threshold location for this largest lobe - which should represent the background.
	if (lobeMap.size() > 1) { return lobeMap.rbegin()->second; }
	
	// If no lobes are found, then the program returns an error code
	else { return -1; }
}

