/****************************************************************************
 *
 *
 * @file threshold.h
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



#ifndef THRESHOLD_H_
#define THRESHOLD_H_



// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "common.h"


// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace cv;
using namespace std;



// ------------------------------------------------------------------------------
//   Defines
// ------------------------------------------------------------------------------

#define MIN_LOBE_AREA 5
#define FIND_WIDTH 10
#define NUM_BINS 256
#define AVERAGE_SIZE 20



// ----------------------------------------------------------------------------------
//   Functions
// ----------------------------------------------------------------------------------

// Converts the give matrix into a histogram
MatND findHistogram(Mat, int);

// Prints a histogram to an image
void showHistogram (MatND inputHistogram, int histSize = NUM_BINS);

// Converts a MatND to a vector
vector<float> histToVector(MatND inputHistogram);

// Takes a moving average filter of the histogram vector
vector<float> movingAverageFilter(vector<float> inputHistogram, int averageSize = AVERAGE_SIZE);

// Finds all local peaks a minimums using a user-defined width
void findPeaksandMins(vector<int>, vector<int>, vector<int>, int);

//Finds and maps all of the lobes in the histogram. Has a default minimum area of 10 pixels.
map<int, int> findBulges(vector<int>, vector<int>, int);


// Application specific function which finds the right most peak, and then searches for the next minimum to set as the threshold value
int findThreshold(Mat);

#endif // THRESHOLD_H_
