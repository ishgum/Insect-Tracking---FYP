#include "IrCam.h"

static Camera camera;
static CameraInfo camInfo;
static Error camError;
static Image rawImage;
static Image rgbImage;
static Format7ImageSettings fmt7ImageSettings;

void PrintError(Error error)
{
	error.PrintErrorTrace();
}

bool irSetROI(Rect ROI) {
	
	camError = camera.StopCapture();
	if (camError != PGRERROR_OK)
	{
		PrintError(camError);
		return false;
	}

	
	fmt7ImageSettings.offsetX = unsigned int (ROI.x);
	fmt7ImageSettings.offsetY = unsigned int (ROI.y);
	fmt7ImageSettings.width = unsigned int (ROI.width);
	fmt7ImageSettings.height = unsigned int (ROI.height);
	
	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	camError = camera.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	// Set the settings to the camera
	camError = camera.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
	if (camError != PGRERROR_OK)
	{
		PrintError(camError);
		return false;
	}

	camError = camera.StartCapture();
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	return true;
}

bool irCamInit(void) {
	// Connect to a camera
	camError = camera.Connect(0);
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	// Get the camera information
	camError = camera.GetCameraInfo(&camInfo);
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	// Start capturing images
	camError = camera.StartCapture();
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	if (!irSetROI(Rect(0, 0, 2048, 2048))) {
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