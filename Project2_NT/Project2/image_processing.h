/* Contains significant image processing components
TODO: check depth works*/

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "histogram.h"
using namespace cv;

Mat preprocessImage(Mat image, bool noBug, int threshFilter[], int thresh_filter_size, int debug_flag, int rgb_flag); // int& threshCount,
vector<Point2f> contourProcessing(Mat dst, Rect ROI, Point xy_loc, int& usable_contours, int noBug, int debug_flag);

vector<Point2f> processFrame(Mat src, Rect ROI, int noBug, int threshFilter[],
	int thresh_filter_size, Point xy_loc, int& usable_contours, int debug_flag, int rgb_flag );

void sourceDisplayAndRecord(Mat src, Rect ROI, VideoWriter outputVideo);

Rect updateROI(Rect ROI, Point stateLoc, Mat src, float roi_size, bool noBug);

#endif