#ifndef IR_CAM_H
#define IR_CAM_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
//#include <Windows.h>
#include "FlyCapture2.h"

#include <iomanip>


using namespace cv;
using namespace std;
using namespace FlyCapture2;

bool irCamInit(void);

bool irCamInitBasic(void);

Mat irGetImage(void);

Mat irGetImageBasic(void);

void irReleaseCam(void);

void irReleaseCamBasic(void);

bool irSetRoi(int offX, int offY, int scale);

#endif
