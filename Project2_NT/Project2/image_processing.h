/* Contains significant image processing components
TODO: check depth works*/

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/imgproc/imgproc.hpp>
#include "histogram.h"
using namespace cv;

Mat preprocessImage(Mat image, bool noBug, int& threshCount, int threshFilter[]);

#endif