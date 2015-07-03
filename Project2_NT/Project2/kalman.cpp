#include "kalman.h"

void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

KalmanFilter setKalmanParameters(KalmanFilter KF) {
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1, 0, 10, 0,
		0, 1, 0, 10,
		0, 0, 1, 0,
		0, 0, 0, 1);

	KF.statePre.at<float>(0) = 0;
	KF.statePre.at<float>(1) = 0;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;

	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-3));
	setIdentity(KF.errorCovPost, Scalar::all(1));
	setIdentity(KF.errorCovPre, Scalar::all(1));
	return KF;
}

void useKalmanFilter(KalmanFilter KF, Point& xy_loc, int& usable_contours, Mat src, Rect ROI, vector<Point2f> mc){
	static Mat measurement = Mat::zeros(2, 1, CV_32F);
	static vector<Point> targetv, kalmanv;
	//Prediction
	Mat predict = KF.predict();
	xy_loc.x = predict.at<float>(0);
	xy_loc.y = predict.at<float>(1);
	//Point xy_loc(predict.at<float>(0), predict.at<float>(1));
	Point xy_vel(predict.at<float>(2), predict.at<float>(3));

	//Attempt to allow tracking of vanishing target
	KF.statePre.copyTo(KF.statePost);
	KF.errorCovPre.copyTo(KF.errorCovPost);

	//Get measurements
	if (usable_contours >= 1) {	// found contours, use contour centres
		measurement.at<float>(0) = mc[0].x + float(ROI.x);
		measurement.at<float>(1) = mc[0].y + float(ROI.y);
	}
	else {						//didn't find anything, use prediction
		measurement.at<float>(0) = xy_loc.x;
		measurement.at<float>(1) = xy_loc.y;
	}

	//Update filter
	Mat correction = KF.correct(measurement);
	Point stateLoc(correction.at<float>(0), correction.at<float>(1));
	Point stateVel(correction.at<float>(2), correction.at<float>(3));
	Point measLoc(measurement.at<float>(0), measurement.at<float>(1));
	targetv.push_back(measLoc);
	kalmanv.push_back(stateLoc);



	// plot stuff
	src = Scalar::all(0); // Creates a frame the size of src, but all black px
	drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
	drawCross(src, measLoc, Scalar(0, 0, 255), 5);

	for (int i = 0; i < targetv.size() - 1; i++)
		line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

	for (int i = 0; i < kalmanv.size() - 1; i++)
		line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

	imshow("Frame Kalman", src);

}