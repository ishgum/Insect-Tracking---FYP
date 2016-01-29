#ifndef THRESHOLD_2_H
#define THRESHOLD_2_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>



#include <iostream>
#include <ctype.h>

#include "common.h"

using namespace cv;
using namespace std;


//#define SHOW_HISTOGRAM
#define MIN_LOBE_AREA 5
#define FIND_WIDTH 10
#define NUM_BINS 256
#define AVERAGE_SIZE 20

MatND findHistogram(Mat, int);



// Takes a moving average filter of the histogram and returns the histogram as a vector of integers
vector<int> movingAverageFilterHistogram(MatND, int);

// Finds all local peaks a minimums using a user-defined width
void findPeaksandMins(vector<int>, vector<int>, vector<int>, int);

//Finds and maps all of the lobes in the histogram. Has a default minimum area of 10 pixels.
map<int, int> findBulges(vector<int>, vector<int>, int);


// Application specific function which finds the right most peak, and then searches for the next minimum to set as the threshold value
int findThreshold(Mat);

#endif
