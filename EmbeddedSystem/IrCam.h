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

#define FULL_FRAME 2048
#define MAX_RATE 90



class IRCam {
	Camera camera;
	CameraInfo camInfo;
	Error camError;
	Image rawImage;
	Image rgbImage;
	Format7ImageSettings fmt7ImageSettings;

	VideoMode vidMode;
	FrameRate fRate;

	Property frmRate;
	Property newFrmRate;
	PGRGuid guid;
	BusManager busMgr;

	unsigned int numCams;
	unsigned int usb_prop;

	unsigned int xRes;
	unsigned int yRes;
	unsigned int fps;
	Mat* srcMat;
public:
	
	IRCam(Mat* src, unsigned int x = FULL_FRAME, unsigned int y = FULL_FRAME, unsigned int frameRate = MAX_RATE);
	~IRCam(void);
	void init();
	void release();

	void getImage();

	Error setROI(int offX, int offY, int scale);
	Error setResolution(unsigned int x, unsigned int y);
	Error setFrameRate ( unsigned int frameRate);
};


bool irCamInit(void);

Mat irGetImage(void);

void irReleaseCam(void);

bool irSetRoi(int offX, int offY, int scale);

#endif
