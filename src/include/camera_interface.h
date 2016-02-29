/****************************************************************************
 *
 *
 * @file camera_interface.h
 *
 * @brief Interface between insect tracker and flycapture software
 *
 * Functions for using the Grasshopper 3 infrared camera with insect tracking software
 *
 * @author Daniel Morris
 * @author Michael McAdam
 *
 *
 ****************************************************************************/



#ifndef CAMERA_INTERFACE_H_
#define CAMERA_INTERFACE_H_


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------


#include <opencv2/core/core.hpp>
#include <numeric> 

//#include <Windows.h>

#include "FlyCapture2.h"
#include "common.h"



// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace cv;
using namespace std;
using namespace FlyCapture2;


// ------------------------------------------------------------------------------
//   Defines
// ------------------------------------------------------------------------------


#define FULL_FRAME 2048
#define MAX_RATE 90
#define FPS_FILTER_SIZE 30



// ----------------------------------------------------------------------------------
//   Camera Interface Class
// ----------------------------------------------------------------------------------

/*
 * Camera Interface Class
 *
 * This provides functions to go through all of the set up steps for a Point
 * Grey Grasshopper 3 Near Infrared camera. It takes a pointer to a matrix
 * so that by calling getImage, the matrix will be automatically updated with
 * the current image.
 * 
 */


class Camera_Interface 
{

private:

	Camera camera;
	Error camError;
	Format7ImageSettings fmt7ImageSettings;

	Image rawImage;
	Image rgbImage;

	Mat* srcMat;

	unsigned int xRes;
	unsigned int yRes;
	unsigned int fps;

	bool initialised;

	map<const char*, int> outputMap;

	vector<float> fpsMA;
	float currentFPS;

public:
	
	Camera_Interface(Mat* src, unsigned int x = FULL_FRAME, unsigned int y = FULL_FRAME, unsigned int frameRate = MAX_RATE);
	~Camera_Interface(void);
	void init();
	void release();

	void getImage();

	Error setROI(int offX, int offY, int scale);
	Error setResolution(unsigned int x, unsigned int y);

	Size getImageSize(void);

	float findFPS(double t1, double t2);
	void updateFPS(float in_fps);

	void printParameters(int y, int x);
	void printOutput(void);

	bool isInit();
};



#endif //CAMERA_INTERFACE_H_
