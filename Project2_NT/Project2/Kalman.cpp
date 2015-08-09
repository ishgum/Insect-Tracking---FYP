#include "Kalman.h"

typedef struct
{
	Mat measurement;
	Point2f stateLoc;
	Point2f stateVel;
	Point2f measLoc;
} KalmanData;

static KalmanData data;

KalmanFilter kalmanInit(void) {
	KalmanFilter KF(4, 2, 0);
	data.measurement = Mat::zeros(2, 1, CV_32F);
	Mat state(4, 1, CV_32F); //x, y, delta x, delta y
	Mat processNoise(4, 1, CV_32F);

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

void kalmanProcess(KalmanFilter* KF, Insect* insect) {
	//Prediction
	Mat predict = KF->predict();
	Point2f xy_loc(predict.at<float>(0), predict.at<float>(1));
	Point2f xy_vel(predict.at<float>(2), predict.at<float>(3));

	//Attempt to allow tracking of vanishing target
	KF->statePre.copyTo(KF->statePost);
	KF->errorCovPre.copyTo(KF->errorCovPost);

	//Get measurements
	if (insect->found) {
		data.measurement.at<float>(0) = insect->position.x;
		data.measurement.at<float>(1) = insect->position.y;
	}
	else {
		data.measurement.at<float>(0) = xy_loc.x;
		data.measurement.at<float>(1) = xy_loc.y;
	}

	//Update filter
	Mat correction = KF->correct(data.measurement);
	data.stateLoc = Point(correction.at<float>(0), correction.at<float>(1));
	data.stateVel = Point(correction.at<float>(2), correction.at<float>(3));
	data.measLoc = Point(data.measurement.at<float>(0), data.measurement.at<float>(1));
}

Point2f kalmanGetStateLoc(void) {
	return data.stateLoc;
}

Point2f kalmanGetStateVel(void) {
	return data.stateVel;
}

Point2f kalmanGetMeasLoc(void) {
	return data.measLoc;
}