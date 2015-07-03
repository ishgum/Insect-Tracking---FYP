#include "image_processing.h"

// Do all improc steps
// return position, arg is frame
vector<Point2f> processFrame(Mat src, Rect ROI, int noBug, int threshFilter[],
	int thresh_filter_size, Point xy_loc, int usable_contours)
{
	Mat src_ROI = src(ROI);

	#ifdef DEBUG
		imshow("Frame", src_ROI);
	#endif // DEBUG

	Mat dst = preprocessImage(src_ROI, noBug, threshFilter, thresh_filter_size);

	Canny(dst, dst, 100, 100 * 2, 3);

	vector<Point2f> mc;
	mc = contourProcessing(dst, ROI, xy_loc, usable_contours, noBug);
	return mc;
}




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
	//MAF and depth binning
	int threshSum = 0;

	for (int p = 0; p < thresh_filter_size; p++) {
		threshSum += threshFilter[p];
	}
	printf("Threshold: %i	", threshSum / thresh_filter_size);
	printf("Height Bracket: %i	", threshSum / (thresh_filter_size * 20));
	// end depth

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

vector<Point2f> contourProcessing(Mat dst, Rect ROI, Point xy_loc, int& usable_contours, int noBug){
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	static Mat measurement = Mat::zeros(2, 1, CV_32F);

	findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}

	///  Get the mass centers:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}


	//Filter by area
	int j = 0;
	vector<vector<Point> > useContours(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		if ((mu[i].m00 < 20000) && (mu[i].m00 > 5)) {
			useContours[j] = vector<Point>(contours[i]);
			j++;
			//printf("Area %i: %.1f ", j, mu[i].m00);
		}
	}
	usable_contours = useContours.size();

	if (j == 0) {
		noBug = true;
	}
	else {
		noBug = false;
	}

#ifdef DEBUG
	/// Draw contours
	Mat drawing = Mat::zeros(dst.size(), CV_8UC3);
	//printf("\nCounter Sizes: ");
	for (int i = 0; i < useContours.size(); i++)
	{
		Scalar color = Scalar(100 * i, 100 * i, 255);
		drawContours(drawing, useContours, i, color, 1, 8, hierarchy, 0, Point());
		//circle(drawing, mc[i], 4, color, -1, 8, 0);
	}

	/// Show in a window

	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
#endif // DEBUG



	return mc;
}
