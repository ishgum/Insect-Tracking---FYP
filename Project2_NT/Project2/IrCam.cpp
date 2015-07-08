#include "IrCam.h"

static Camera camera;
static CameraInfo camInfo;
static Error camError;
static Image rawImage;
static Image rgbImage;

bool irCamInit(void) {

	// Connect the camera
	camError = camera.Connect(0);
	if (camError.operator!=(PGRERROR_OK))
	{
		cout << "Failed to connect to camera" << endl;
		return false;
	}

	// Get the camera info and print it out
	camError = camera.GetCameraInfo(&camInfo);
	if (camError.operator!=(PGRERROR_OK))
	{
		cout << "Failed to get camera info from camera" << endl;
		return false;
	}

	cout << camInfo.vendorName << " "
		 << camInfo.modelName << " "
		 << camInfo.serialNumber << endl;

	camError = camera.StartCapture();
	if (camError == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED)
	{
		cout << "Bandwidth exceeded" << endl;
		return false;
	}
	else if (camError.operator!=(PGRERROR_OK))
	{
		cout << "Failed to start image capture" << endl;
		return false;
	}
	return true;
}

Mat irGetImage(void) {
	Mat img;
	// Get the image
	camError = camera.RetrieveBuffer(&rawImage);
	
	if (camError.operator!=(PGRERROR_OK)) {
		cout << "capture error" << endl;
		img.setTo(-1);
		return img;
	}

	// Convert to rgb
	rawImage.Convert(PIXEL_FORMAT_BGR, &rgbImage);

	// Convert to OpenCV Mat
	unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
	img = Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);

	return img;
}

void irReleaseCam(void) {
	camError = camera.StopCapture();
	if (camError.operator!=(PGRERROR_OK))
	{
	// This may fail when the camera was removed, so don't show
	// an error message
	}

	camera.Disconnect();
}