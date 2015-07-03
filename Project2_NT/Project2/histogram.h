#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

MatND findHistogram(Mat inputImage, int debug_flag, int numBins = 256);
vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = 1);
void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = 15);
int findThresholdByArea(vector<int> inputHistogram, int minArea = 30);
int findThreshold(Mat inputImage, int previousThreshold, bool& noBug, int debug_flag);

#endif