// display the difference between adjacent video frames (press any key to exit)
#include <opencv\cv.h>
#include <opencv\highgui.h>
int main()
{
	CvCapture* capture = cvCreateCameraCapture(0); // connect to the camera
	IplImage* image = cvQueryFrame(capture); // get the first frame of video
	IplImage* previousFrame = cvCloneImage(image); // save it to the previous frame
	IplImage* differenceImage = cvCloneImage(image); // allocate difference image
	while (cvWaitKey(10) < 0)
	{
		image = cvQueryFrame(capture); // get the next frame of video
		cvAbsDiff(image, previousFrame, differenceImage);//calculate difference image
		cvThreshold(differenceImage, differenceImage, 32, 0, CV_THRESH_TOZERO); //threshold
		cvThreshold(differenceImage, differenceImage, 0, 255, CV_THRESH_BINARY);//highlight
		// insert one line of code here to median filter out noise in difference image:
		cvShowImage("difference images", differenceImage); // display difference image
		cvCopy(image, previousFrame); // backup current frame
	}
	cvReleaseCapture(&capture); // release the camera
	return 0;
}