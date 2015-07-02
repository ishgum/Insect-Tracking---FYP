#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

MatND findHistogram(Mat inputImage, int numBins = 256);
vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = 1);
void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = 15);
int findThresholdByArea(vector<int> inputHistogram, int minArea = 30);
int findThreshold(Mat inputImage, int previousThreshold, bool& noBug);

#endif