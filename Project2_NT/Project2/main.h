#ifndef MAIN_H
#define MAIN_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

using namespace cv;
using namespace std;

#include "fps.h"
#include "histogram.h"
#include "kalman.h"
#include "image_processing.h"

#define RGB_SOURCE		// preformance increase when left undefined for a greyscale source (skips HSL conv)
#define ROI_SIZE .15
#define DEBUG		//display video output windows
#define FPS //wall breaks (==0) on release mode. !When FPS defined && DEBUG undefined release mode breaks
#define KALMAN
#define HEIGHT_OFFSET 10
//#define RECORD_SOURCE_W_BOX		//record source footage with ROI overlay
#define FIND_DEPTH
#define WAIT_PERIOD	10
#define THRESH_FILTER_SIZE 10						// size of MAF for depth value

int threshFilter[THRESH_FILTER_SIZE] = { 0 };		// MAF for depth values

bool noBug = false;

#endif