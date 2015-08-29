
#include "Thresholding.h"

//#define SHOWHISTOGRAM

#ifdef THRESH_GPU
Mat findHistogram(GpuMat inputImage, int numBins = 256) {
	//Mat hist;
	//Mat histNormal;
	GpuMat hist;
	GpuMat histNormal;


	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	//calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	calcHist(inputImage, hist);

	// Download histogram to CPU
	Mat cpuHist(hist);

#ifdef SHOWHISTOGRAM
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	//Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	GpuMat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, histNormal, 0, histImage.rows, NORM_MINMAX, -1, Mat());	//Same function call for GPU & CPU

	Mat histImg(histImage);
	Mat histNrml(histNormal);

	for (int i = 1; i < histSize; i++)
	{
		line(histImg, Point(bin_w*(i - 1), hist_h - cvRound(histNrml.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNrml.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImg);
#endif // SHOWHISTOGRAM


	return cpuHist;
}

#else
Mat findHistogram(Mat inputImage, int numBins = 256) {
	Mat hist;
	Mat histNormal;


	/// Establish the number of bins
	int histSize = numBins;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	calcHist(&inputImage, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

#ifdef SHOWHISTOGRAM
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, histNormal, 0, histImage.rows, NORM_MINMAX, -1, Mat());


	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histNormal.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNormal.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);
#endif // SHOWHISTOGRAM


	return hist;
}
#endif // THRESH_GPU

myHist::myHist() {
	absMaxLoc = 0;
}

myHist::myHist(Mat inputHistogram) {
	inputHistogram.col(0).copyTo(histogram);
	absMaxLoc = 0;
}

void myHist::findPeaksandMins(int findWidth = 15) {

	int forLoopMin = -findWidth;
	int forLoopMax = findWidth;

	for (int i = 0; i < histogram.size(); i++) {

		//Set for loop min and maximums
		if (i < findWidth) {
			forLoopMin = -i;
			forLoopMax = findWidth;
		}
		else if ((i + findWidth) > histogram.size()) {
			forLoopMax = int(histogram.size()) - i;
			forLoopMin = -findWidth;
		}
		else {
			forLoopMin = -findWidth;
			forLoopMax = findWidth;
		}


		//If current position is the current min or max, save it's position.
		double currentMin = histogram[i];
		double currentMax = histogram[i];

		for (int k = forLoopMin; k < forLoopMax; k++) {
			if (histogram[i + k] < currentMin) {
				currentMin = histogram[i + k];
			}
			if (histogram[i + k] > currentMax) {
				currentMax = histogram[i + k];
			}
		}

		if (currentMin == histogram[i]) {
			minLocs.push_back(i);
		}
		if (currentMax == histogram[i] && currentMax != 0) {
			peakLocs.push_back(i);
		}

		if (histogram[i] > histogram[absMaxLoc]) {
			absMaxLoc = i;
		}
	}
}


void myHist::findThresholdByArea(int minArea = 30)
{
	double threshSum = 0;
	int i = 0;
	bool areaFound = false;

	for (i = int(histogram.size()) - 1; i > 0 && areaFound == false; i--) {
		threshSum += histogram[i];

		if (threshSum > minArea)
		{
			areaFound = true;
		}
	}

	if (checkThreshold(i)) {
		threshold = findNextMin(i);
	}
	else {
		threshold = -1;
	}
}

bool myHist::checkThreshold(int testThreshold) {
	return !((testThreshold - 10) < absMaxLoc);
}

int myHist::findNextMin(int testThreshold) {
	for (int i = int(minLocs.size()) - 1; i >= 0; i--) {
		if (minLocs[i] < testThreshold) {
			return minLocs[i];
		}
	}
	return -1;
}

#ifdef THRESH_GPU
int findThreshold(GpuMat inputImage) {

	myHist hist(findHistogram(inputImage));
	hist.findPeaksandMins();
	hist.findThresholdByArea();

	//printf("Threshold: %i ", hist.threshold);
	return hist.threshold;

}

#else
int findThreshold(Mat inputImage) {

	myHist hist(findHistogram(inputImage));
	hist.findPeaksandMins();
	hist.findThresholdByArea();

	//printf("Threshold: %i ", hist.threshold);
	return hist.threshold;

}
#endif
