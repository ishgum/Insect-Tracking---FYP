#include "IrCam.h"

static Camera camera;
static CameraInfo camInfo;
static Error camError;
static Image rawImage;
static Image rgbImage;
static Format7ImageSettings fmt7ImageSettings;

static VideoMode vidMode;
static FrameRate fRate;

static Property frmRate;
static Property newFrmRate;
static PGRGuid guid;
static BusManager busMgr;

/*NEW*/
static Error basicError;
static Image basicRaw;
static Camera basicCamera;
static CameraInfo basicCamInfo;
static Image rgbImageBasic;

/* Set a region of interest on the camera */
bool irSetRoi(int offX, int offY, int scale) {

	fmt7ImageSettings.offsetX = offX;
	fmt7ImageSettings.offsetY = offY;
	fmt7ImageSettings.width = (unsigned int)2048 / scale;
	fmt7ImageSettings.height = (unsigned int)2048 / scale;

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
		return false;
	}

	return true;
}

/*Sets the resolution of the camera to a low value
  (nominally 640 by 480) and lowers the framerate
  (nominal value of 7.5 FPS).
*/
bool setLowResFrameRate(void) {
	// Set the resolution
	fmt7ImageSettings.width = (unsigned int) 480;
	fmt7ImageSettings.height = (unsigned int) 640;
	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings
	camError = camera.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);
	if (camError != PGRERROR_OK)
	{
		return false;
	}

	// Set the settings on the camera
	camError = camera.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
	if (camError != PGRERROR_OK)
	{
        cout << "Failed to set resolution" << endl;
		return false;
	}

	//cout << "Set lower resolution" << endl;

	// Set the framerate

	frmRate.type = FRAME_RATE;
	frmRate.absControl = true;
	frmRate.onePush = false;
	frmRate.onOff = true;
	frmRate.autoManualMode = false;
	frmRate.absValue = 30.0;

	camError = camera.SetProperty(&frmRate);

	if (camError != PGRERROR_OK)
	{
		cout << "Failed to set framerate" << endl;
		return false;
	}


	// Get current framerate to check
	newFrmRate.type = FRAME_RATE;
	camError = camera.GetProperty(&newFrmRate);
	if (camError != PGRERROR_OK)
	{
		cout << "Failed to get framerate" << endl;
		return false;
	}

	cout << "Frame rate is " << fixed << setprecision(2) << newFrmRate.absValue << " fps" << endl;

	return true;
}

/* Initialise the camera and set ROI to full frame */
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
	cout << "Set ROI" << endl;
	if (!irSetRoi(0, 0, 1)) {
		return false;
	}

	if (!setLowResFrameRate()) {
		return false;
	}

	// Start capturing images
	camError = camera.StartCapture();
	cout << "Capturing" << endl;
	if (camError != PGRERROR_OK)
	{
		return false;
	}
	return true;
}

/* Initialise the camera and set ROI to full frame */
bool irCamInitBasic(void) {
	// Connect to a camera
	basicError = basicCamera.Connect(0);
	if (basicError != PGRERROR_OK)
	{
		return false;
	}

	// Get the camera information

	basicError = basicCamera.GetCameraInfo(&basicCamInfo);

	if (basicError != PGRERROR_OK)
	{
		return false;
	}
	// Start capturing images
	basicError = basicCamera.StartCapture();
	cout << "Capturing" << endl;
	if (basicError != PGRERROR_OK)
	{
		return false;
	}
	return true;
}

/* Retrieve an image from the camera and return as an OpenCV Mat */
Mat irGetImageBasic(void) {
	Mat img;
	// Get the image
    
	//cout << "Capturing image" << endl;
	basicError = basicCamera.RetrieveBuffer(&basicRaw);
	//cout << "Got an image" << endl;
	
	if (basicError.operator!=(PGRERROR_OK)) {
		cout << "capture error" << endl;
		img.setTo(-1);
		return img;
	}

	// Convert to rgb
	basicRaw.Convert(PIXEL_FORMAT_BGR, &rgbImageBasic);

	// Convert to OpenCV Mat
	unsigned int rowBytes = (double)rgbImageBasic.GetReceivedDataSize() / (double)rgbImageBasic.GetRows();
	img = Mat(rgbImageBasic.GetRows(), rgbImageBasic.GetCols(), CV_8UC3, rgbImageBasic.GetData(), rowBytes);

	return img;
}

/* Retrieve an image from the camera and return as an OpenCV Mat */
Mat irGetImage(void) {
	Mat img;
	// Get the image
    
	cout << "Capturing image" << endl;
	camError = camera.RetrieveBuffer(&rawImage);
	cout << "Got an image" << endl;
	
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

/* Release the camera */
void irReleaseCamBasic(void) {
	basicError = basicCamera.StopCapture();
	if (camError.operator!=(PGRERROR_OK))
	{
	// This may fail when the camera is removed, so don't show
	// an error message
	}
	basicCamera.Disconnect();
}

/* Release the camera */
void irReleaseCam(void) {
	camError = camera.StopCapture();
	if (camError.operator!=(PGRERROR_OK))
	{
	// This may fail when the camera is removed, so don't show
	// an error message
	}
	camera.Disconnect();
}
