#include "IrCam.h"


#define FPS_FILTER_SIZE 30



IRCam::IRCam(Mat* src, unsigned int x, unsigned int y, unsigned int frameRate) {

	xRes = x;
	yRes = y;
	fpsMA = vector<float>(FPS_FILTER_SIZE, 0);
	fps = fps;
	currentFPS = 0.0;
	srcMat = src;
	initialised = false;
}

IRCam::~IRCam ( void ) {
	wprintw(output.outputStream, "Camera closing\n");	
	release();

}


void IRCam::init(void) {
	


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
	camError = setROI(0, 0, 1);
	if (camError != PGRERROR_OK) 	{ throw "Set ROI"; }

	// Start capturing images
	camError = camera.StartCapture();
	if (camError != PGRERROR_OK)	{ throw "Start Capture"; }


	camError = busMgr.GetNumOfCameras(&numCams);
	if (camError != PGRERROR_OK)	{ throw "Num Cams"; }

	camError = busMgr.GetCameraFromIndex(0, &guid);
	if (camError != PGRERROR_OK)	{ throw "Camera Index"; }


	camError = busMgr.GetUsbPortStatus(guid, &usb_prop);
	//if (camError != PGRERROR_OK)	{throw "USB prop"; }
	wprintw(output.outputStream, "USB is %u\n", usb_prop);

	initialised = true;
	return;
}




Error IRCam::setROI(int offX, int offY, int scale) {

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
	if (camError != PGRERROR_OK)
	{
		return camError;
	}

	// Set the settings to the camera
	camError = camera.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
	if (camError != PGRERROR_OK)
	{
		return camError;
	}

	return camError;
}



Error IRCam::setResolution(unsigned int x, unsigned int y) {
	// Set the resolution
	

	fmt7ImageSettings.width = x;
	fmt7ImageSettings.height = y;

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
	if (camError != PGRERROR_OK) { return camError; }
	
	return camError;
}




Error IRCam::setFrameRate ( unsigned int frameRate) {
	// Lower the framerate

	frmRate.type = FRAME_RATE;
	frmRate.absControl = true;
	frmRate.onePush = false;
	frmRate.onOff = true;
	frmRate.autoManualMode = false;
	frmRate.absValue = 30.0;

	camError = camera.SetProperty(&frmRate);

	if (camError != PGRERROR_OK) { return camError; }


	// Get current framerate to check
	newFrmRate.type = FRAME_RATE;
	camError = camera.GetProperty(&newFrmRate);
	if (camError != PGRERROR_OK) { return camError; }

	cout << "Frame rate is " << fixed << setprecision(2) << newFrmRate.absValue << " fps" << endl;

	return camError;
}


void IRCam::getImage(void) {

	// Get the image
	//cout << endl << "Capturing image...";
	camError = camera.RetrieveBuffer(&rawImage);
	//cout << "Got an image" << endl;
	
	if (camError != PGRERROR_OK) { throw "Failed Capture"; }

	// Convert to rgb
	rawImage.Convert(PIXEL_FORMAT_BGR, &rgbImage);

	// Convert to OpenCV Mat
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
	*srcMat = Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);

}

void IRCam::release(void) {
	camError = camera.StopCapture();
	camera.Disconnect();
}


Size IRCam::getImageSize(void) {
	return Size(xRes, yRes);
}



bool IRCam::isInit(void) {
	return initialised;
}


void IRCam::updateFPS(float in_fps)
{
	fpsMA.push_back(in_fps);
	fpsMA.erase(fpsMA.begin());

	currentFPS = std::accumulate(fpsMA.begin(), fpsMA.end(), 0) / (FPS_FILTER_SIZE);
}


void IRCam::printOutput(void) {

	werase(output.cameraData);
	wprintw(output.cameraData, "%d\n", initialised);
	wprintw(output.cameraData, "%.2f \n", currentFPS);
	wprintw(output.cameraData, "%d X %d\n", xRes, yRes);

}





