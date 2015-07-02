#ifndef HISTOGRAM_H
#define HISTOGRAM_H

//for findhisto
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/video/tracking.hpp>
//#include <iostream>
//#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <ctype.h>

//using namespace std;

MatND findHistogram(Mat inputImage, int numBins = 256);
vector<int> movingAverageFilterHistogram(MatND histogramInput, int averageSize = 1);
void findPeaksandMins(vector<int> inputHistogram, vector<int>& peaks, vector<int>& mins, int findWidth = 15);
int findThresholdByArea(vector<int> inputHistogram, int minArea = 30);
int findThreshold(Mat inputImage, int previousThreshold, bool& noBug);

#endif