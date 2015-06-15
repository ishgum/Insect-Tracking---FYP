#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
	Mat img;

	img = imread("C:/Users/myadmin/Documents/Image/TestImageInfraRed.jpg", CV_LOAD_IMAGE_COLOR);

	while (waitKey(10) < 0)
	{
		Mat imgHLS;

		cvtColor(img, imgHLS, COLOR_BGR2HLS); //Convert the captured frame from BGR to HSV

		vector<Mat> channels;
		split(imgHLS, channels);

		imshow("L original", channels[1]);

		Mat L = channels[1];

		medianBlur(L, L, 3);

		double min, max;
		Point minPoint, maxPoint;
		minMaxLoc(L, &min, &max, &minPoint, &maxPoint);

		Mat squares = Mat::zeros(L.size(), CV_8UC3);

		// Is it possible to draw a grayscale rectangle (cannot add three-channel square to single channel V)
		rectangle(squares,
			cvPoint(maxPoint.x - 15 / 2, maxPoint.y - 15 / 2),
			cvPoint(maxPoint.x + 15 / 2, maxPoint.y + 15 / 2 / 2),
			CV_RGB(0, 255, 0), 1, 8
			);

		imshow("Square", squares);

		imgHLS = img + squares;

		imshow("Brightest point", imgHLS);
	}
	return 0;
}

