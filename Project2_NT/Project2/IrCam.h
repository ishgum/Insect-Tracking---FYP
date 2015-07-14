#ifndef IR_CAM_H
#define IR_CAM_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <Windows.h>
#include "FlyCapture2.h"


using namespace cv;
using namespace std;
using namespace FlyCapture2;

bool irCamInit(void);

Mat irGetImage(void);

void irReleaseCam(void);

bool irSetROI(Rect ROI);

#endif