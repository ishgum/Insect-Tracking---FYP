#include "Insect.h"

#define HEIGHT_FILTER_SIZE 10
#define HEIGHT_BRACKET 20




Insect::Insect(Size in_size) {
	found = false;
	heightMA = vector<int>(HEIGHT_FILTER_SIZE, 0);
	position = Point(0, 0);
	prevPosition = position;
	speed = 0.0;
	size = in_size;
	frameCentre = Point(size.width / 2, size.height / 2);
}



void Insect::updateHeight(int brightness) {
	heightMA.push_back(brightness);
	heightMA.erase(heightMA.begin());

	heightBracket = std::accumulate(heightMA.begin(), heightMA.end(), 0) / (HEIGHT_FILTER_SIZE * HEIGHT_BRACKET);
}


void Insect::printOutput(void) {

	werase(output.insectData);
	wprintw(output.insectData, "%d\n", found);
	wprintw(output.insectData, "%.2f\n", relPosition.x);
	wprintw(output.insectData, "%.2f\n", relPosition.y);
	wprintw(output.insectData, "%d\n", heightBracket);
	wprintw(output.insectData, "%.2f\n", relAngle);
	wprintw(output.insectData, "%d\n", found);
}





void Insect::updatePosition(Point2f centre) {
	position = Point2f(centre.x + ROI.x, centre.y + ROI.y);
	
	velocity = position - prevPosition;
	speed = norm(velocity);
	direction = atan2f(velocity.y, velocity.x) * 180 / CV_PI;

	relPosition = position - frameCentre;
	relNorm = norm(relPosition);
	relAngle = atan2f(relPosition.y, relPosition.x) *180 / CV_PI;

	prevPosition = position;
}




void Insect::updateROI(void) {
	if (found) {
		// Updates the size and location of the region of interest
		int roiSize = ROI_SIZE * size.height;

		if (position.x > (roiSize / 2)) {
			ROI.x = int(position.x) - roiSize / 2;
		}
		if (position.y > (roiSize / 2)) {
			ROI.y = int(position.y) - roiSize / 2;
		}
		if ((position.x + roiSize / 2) >= size.width) {
			ROI.x = size.width - roiSize;
		}
		if ((position.y + roiSize / 2) >= size.height) {
			ROI.y = size.height - roiSize;
		}
		ROI.width = roiSize;
		ROI.height = roiSize;
	}
	else {
		ROI = Rect(WIDTH_OFFSET, HEIGHT_OFFSET, size.width - WIDTH_OFFSET, size.height - HEIGHT_OFFSET); //Reset ROI
	}
}




vector<vector<Point> > Insect::findObjects(Mat* inputImage) {
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(*inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	return contours;
}



map<double, vector<Point> > Insect::mapContours(vector<vector<Point> > inputContours, Mat* inputImage) {
	map<double, vector<Point> > contourMap;

	for (int i = 0; i < inputContours.size(); i++)
	{
		if (inputContours[i].size() > 10) 
		{
			Mat mask = Mat::zeros(inputImage->size(), CV_8UC1);
			drawContours(mask, inputContours, i, cv::Scalar(255), CV_FILLED, CV_AA, noArray(), 1, Point(0, 0));
			contourMap[mean(*inputImage, mask)[0]] = inputContours[i];
		}
	}
	return contourMap;
}



void Insect::findInsect(Mat* inputImage) {
	Mat values[3], lum;
	split(*inputImage, values);						// Split into channels
	lum = values[0];

	int lumThreshold = findThreshold(lum);		//Perform Dynamic thresholding on the saturation image
	
	if (lumThreshold < 0) {
		found = false;
		return;
	}

	threshold(lum, lum, lumThreshold, 255, 0);
	//threshold(lum, lum, 200, 255, 0);

	vector<vector<Point> > imageContours = findObjects(&lum);
	map<double, vector<Point> > contourMap = mapContours(imageContours, &lum);
		

	if (contourMap.size() != 0) {
		double insectIntensity = contourMap.rbegin()->first;
		Moments conMom = (moments(contourMap.rbegin()->second, false));
		Point insectCentre = Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00);

		updatePosition(insectCentre);
		found = true;
	}
	else {
		found = false;
	}
}
















