#include "image_processing.h"

// Do all improc steps
// return position, arg is frame
//processFrame(){
//
//}

//void display


Mat preprocessImage(Mat image, bool noBug, int threshFilter[], int thresh_filter_size) {
	static int threshCount = 0;

	Mat values[3]; Mat image_hsl; Mat dst;
	static int lumThreshold = 0;

	// Conversion uses significant processor time,
	// using point grey camera we should be able to skip this step,
	// as it proivdes only one 'brightness' channel.
	#ifdef RGB_SOURCE
		cvtColor(image, image, CV_BGR2HLS);		// Convert image to HSL
	#endif
	split(image, values);						// Split into channels
	//printf("\n\nimage chan:\t%d", image.channels());
	Mat lum = values[1];
	//printf("\nimage chan:\t%d\n\n", lum.channels());
	medianBlur(lum, dst, 1);
	lumThreshold = findThreshold(dst, lumThreshold, noBug);		//Perform Dynamic thresholding on the saturation image
	threshFilter[threshCount++] = lumThreshold;
	if (threshCount == thresh_filter_size)
	{
		threshCount = 0;
	}

	MatND hist;
	MatND histNormal;
	/// Establish the number of bins
	int histSize = 256;
	float range[] = { lumThreshold, 256 };
	const float* histRange = { range };

	calcHist(&dst, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, histNormal, 0, histImage2.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage2, Point(bin_w*(i - 1), hist_h - cvRound(histNormal.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(histNormal.at<float>(i))),
			Scalar(255, 255, 255), 1, 8, 0);
	}

	#ifdef DEBUG
		namedWindow("calcHist Demo2", CV_WINDOW_AUTOSIZE);
		imshow("calcHist Demo2", histImage2);
	#endif
		threshold(dst, dst, lumThreshold, 255, 0);

	#ifdef DEBUG
		imshow("Luminance", dst);
	#endif // DEBUG

	GaussianBlur(dst, dst, Size(11, 11), 2);
	threshold(dst, dst, 5, 255, 0);

	return dst;
}
