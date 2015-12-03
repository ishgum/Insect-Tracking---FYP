#ifndef KALMAN_H
#define KALMAN_H

#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include "Insect.h"

using namespace cv;

KalmanFilter kalmanInit(void);

void kalmanProcess(KalmanFilter* KF, Insect* insect);

Point2f kalmanGetStateLoc(void);

Point2f kalmanGetStateVel(void);

Point2f kalmanGetMeasLoc(void);

#endif //KALMAN_H