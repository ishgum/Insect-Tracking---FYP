#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

//#define HIST
#ifdef HIST

int main(void)
{
	Mat frame, src_hsl;
	VideoCapture capture(0);

	while (waitKey(10) < 0) {
		capture >> frame;

		Mat values[3];

		// Convert image to HSL
		cvtColor(frame, src_hsl, CV_BGR2HLS);
		// Split into channels
		split(src_hsl, values);
		// Get luminance channel
		Mat lum = values[1];

		Mat lum_hist;
		// Establish the number of bins
		int histSize = 256;
		// Set the range
		float range[] = { 0, 256 };
		const float* histRange = { range };

		calcHist(&lum, 1, 0, Mat(), lum_hist, 1, &histSize, &histRange, true, false);


		// Draw the histogram for L
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / histSize);

		Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		// Normalize the result to [ 0, histImage.rows ]
		normalize(lum_hist, lum_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

		for (int i = 1; i < histSize; i++)
		{
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(lum_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(lum_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
		}

		// Display
		namedWindow("Luminance histogram", CV_WINDOW_AUTOSIZE);
		imshow("Luminance histogram", histImage);

		namedWindow("Vid feed", CV_WINDOW_AUTOSIZE);
		imshow("Vid feed", frame);
	}
	return 0;
}

#endif