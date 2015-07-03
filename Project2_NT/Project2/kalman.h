#ifndef KALMAN_H
#define KALMAN_H

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void drawCross(Mat img, Point centre, Scalar colour, int d);

KalmanFilter setKalmanParameters(KalmanFilter KF);

void useKalmanFilter(KalmanFilter KF, Point& xy_loc, int& usable_contours, Mat src, Rect ROI, vector<Point2f> mc, int debug_flag);

#endif