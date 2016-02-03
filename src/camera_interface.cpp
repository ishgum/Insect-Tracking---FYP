/****************************************************************************
 *
 *
 * @file camera_interface.cpp
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


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include "camera_interface.h"


// ----------------------------------------------------------------------------------
//   Camera Interface Class
// ----------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Con/De structors
// ------------------------------------------------------------------------------


// Note that this defaults to the full frame and the max frame rate

Camera_Interface::Camera_Interface(Mat* src, unsigned int x, unsigned int y, unsigned int frameRate) 
{
	xRes = x;		// Set the user defined x and y resolutions and frame rate
	yRes = y;
	fps = frameRate;
	srcMat = src;		// Set the matrix output to the specified user frame
	initialised = false;

	fpsMA = vector<float>(FPS_FILTER_SIZE, 0);
	currentFPS = 0.0;
}

Camera_Interface::~Camera_Interface ( void ) 
{
	wprintw(output.outputStream, "Camera closing\n");	
	camError = camera.StopCapture();
	camera.Disconnect();
}


// ------------------------------------------------------------------------------
//   Methods
// ------------------------------------------------------------------------------


// ------------------------------------------------------------------------------
//   Initialise Camera
// ------------------------------------------------------------------------------

void 
Camera_Interface::init(void) 
{
	// Temporary variables needed for intialisation
	CameraInfo camInfo;
	PGRGuid guid;
	BusManager busMgr;

	unsigned int numCams;
	unsigned int usb_prop;
	
	// Connect to a camera
	camError = camera.Connect(0);
	if (camError != PGRERROR_OK)	{ throw "Camera Connect"; }

	// Get the camera information
	camError = camera.GetCameraInfo(&camInfo);
	if (camError != PGRERROR_OK)	{ throw "Camera Info"; }

	// Set the Resolution
	camError = setResolution(xRes, yRes);
	if (camError != PGRERROR_OK)	{ throw "Set Resolution"; }

	// Set ROI
	camError = setROI(0, 0, 1);					// Note that the region of interest is always set to the full screen
	if (camError != PGRERROR_OK) 	{ throw "Set ROI"; }

	// Start capturing images
	camError = camera.StartCapture();
	if (camError != PGRERROR_OK)	{ throw "Start Capture"; }

	// Get number of cameras
	camError = busMgr.GetNumOfCameras(&numCams);
	if (camError != PGRERROR_OK)	{ throw "Num Cams"; }

	// Get camera index
	camError = busMgr.GetCameraFromIndex(0, &guid);
	if (camError != PGRERROR_OK)	{ throw "Camera Index"; }

	// Print the usb port status
	camError = busMgr.GetUsbPortStatus(guid, &usb_prop);
	wprintw(output.outputStream, "USB is %u\n", usb_prop);

	// If the camera succesfully reaches this point, it should be intialised
	initialised = true;
	return;
}



// ------------------------------------------------------------------------------
//   Set Hardware Region of Interest
// ------------------------------------------------------------------------------

Error Camera_Interface::setROI(int offX, int offY, int scale) 
{
	fmt7ImageSettings.offsetX = offX;
	fmt7ImageSettings.offsetY = offY;
	fmt7ImageSettings.width = (unsigned int)xRes / scale;
	fmt7ImageSettings.height = (unsigned int)yRes / scale;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	camError = camera.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);

	if (camError != PGRERROR_OK) { return camError; }

	// Set the settings to the camera
	camError = camera.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);

	if (camError != PGRERROR_OK) { return camError;	}

	return camError;
}



// ------------------------------------------------------------------------------
//   Set Resolution
// ------------------------------------------------------------------------------

Error Camera_Interface::setResolution(unsigned int x, unsigned int y) 
{
	fmt7ImageSettings.width = x;
	fmt7ImageSettings.height = y;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	camError = camera.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);


	// Make sure to return if format settings arent valid
	if (camError != PGRERROR_OK) { return camError; }	


	// Set the settings to the camera
	camError = camera.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
	
	return camError;
}




// ------------------------------------------------------------------------------
//   Get Image
// ------------------------------------------------------------------------------

void Camera_Interface::getImage(void) 
{
	// Retrieve buffer from camera
	camError = camera.RetrieveBuffer(&rawImage);
	
	// Throw error if capture fails
	if (camError != PGRERROR_OK) { throw "Failed Capture"; }

	// Convert to RGB
	rawImage.Convert(PIXEL_FORMAT_BGR, &rgbImage);


	// Convert to OpenCV Matrix
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
	*srcMat = Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);

	return;
}


// ------------------------------------------------------------------------------
//   Get user set image size
// ------------------------------------------------------------------------------

Size Camera_Interface::getImageSize(void) 
{
	return Size(xRes, yRes);
}


// ------------------------------------------------------------------------------
//   Get if camera is intialised
// ------------------------------------------------------------------------------

bool Camera_Interface::isInit(void) 
{
	return initialised;
}


// ------------------------------------------------------------------------------
//   Find FPS
// ------------------------------------------------------------------------------

float Camera_Interface::findFPS(float t1, float t2)   
{   

	float diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC );   
	float fps = 1/diff;

	return fps;   
}


// ------------------------------------------------------------------------------
//   Update FPS
// ------------------------------------------------------------------------------

void Camera_Interface::updateFPS(float in_fps)
{

	fpsMA.push_back(in_fps);
	fpsMA.erase(fpsMA.begin());

	currentFPS = std::accumulate(fpsMA.begin(), fpsMA.end(), 0) / (FPS_FILTER_SIZE);
}




// ------------------------------------------------------------------------------
//   Output parameters to ncurses window
// ------------------------------------------------------------------------------

void Camera_Interface::printParameters(int y, int x)
{
	outputMap["Camera Info"] = 0;

	outputMap["Camera Connected:"] = 2;
	outputMap["FPS:"] = 3;
	outputMap["Frame Size:"] = 4;

	printDataWindow(outputMap, y, x);

}

// ------------------------------------------------------------------------------
//   Refresh data in ncurses window
// ------------------------------------------------------------------------------

void Camera_Interface::printOutput(void) 
{
	werase(output.cameraData);
	mvwprintw(output.cameraData, outputMap["Camera Connected:"], 0, "%d\n", initialised);
	mvwprintw(output.cameraData, outputMap["FPS:"], 0, "%.2f \n", currentFPS);
	mvwprintw(output.cameraData, outputMap["Frame Size:"], 0, "%d X %d\n", xRes, yRes);
}






