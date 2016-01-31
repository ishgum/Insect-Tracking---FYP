/****************************************************************************
 *
 *
 * @file insect.cpp
 *
 * @brief Insect class which contains all of the data and methods for locating the insect
 *
 * Functions for finding the insect from an image as well as for encapsulating data for the
 * insect that is found.
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include "insect.h"



// ----------------------------------------------------------------------------------
//   Insect Class
// ----------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Constructor
// ------------------------------------------------------------------------------


Insect::Insect(Size in_size) 
{
	found = false;
	heightMA = vector<int>(HEIGHT_FILTER_SIZE, 0);
	position = Point(0, 0);
	prevPosition = position;
	speed = 0.0;
	size = in_size;
	frameCentre = Point(size.width / 2, size.height / 2);

}



// ------------------------------------------------------------------------------
//   Methods
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Update the moving average for the insect height
// ------------------------------------------------------------------------------

void Insect::updateHeight(int brightness) 
{
	heightMA.push_back(brightness);		// Note, this assumes that height directly correlates to the brightness of the insect
	heightMA.erase(heightMA.begin());

	// Sum the vector and average by number of entries
	heightBracket = std::accumulate(heightMA.begin(), heightMA.end(), 0) / (HEIGHT_FILTER_SIZE * HEIGHT_BRACKET);
}


// ------------------------------------------------------------------------------
//   Update position
// ------------------------------------------------------------------------------

void Insect::updatePosition(Point2f centre) 
{
	// Position must take into account the current position of the ROI 
	position = Point2f(centre.x + ROI.x, centre.y + ROI.y);
	
	// A velocity vector is calculated by the difference between positions
	velocity = position - prevPosition;
	speed = norm(velocity);
	direction = atan2f(velocity.y, velocity.x) * 180 / CV_PI;

	// Find position relative to the centre
	relPosition = position - frameCentre;
	relNorm = norm(relPosition);
	relAngle = atan2f(relPosition.y, relPosition.x) *180 / CV_PI;

	// Store position for next iteration
	prevPosition = position;
}



// ------------------------------------------------------------------------------
//   Update insect ROI
// ------------------------------------------------------------------------------

void Insect::updateROI(void) 
{
	if (found) 
	{
		// Updates the size and location of the region of interest
		int roiSize = ROI_SIZE * size.height;

		// If the top or left side is greater than zero, assign new ROI position
		if (position.x > (roiSize / 2)) {
			ROI.x = int(position.x) - roiSize / 2;
		}
		if (position.y > (roiSize / 2)) {
			ROI.y = int(position.y) - roiSize / 2;
		}

		// If the right or bottom side is greater than the frame, set to sit on the edge
		if ((position.x + roiSize / 2) >= size.width) {
			ROI.x = size.width - roiSize;
		}
		if ((position.y + roiSize / 2) >= size.height) {
			ROI.y = size.height - roiSize;
		}

		ROI.width = roiSize;
		ROI.height = roiSize;
	}

	else 
	{
		//Reset ROI to the full frame
		ROI = Rect(WIDTH_OFFSET, HEIGHT_OFFSET, size.width - WIDTH_OFFSET, size.height - HEIGHT_OFFSET); 
	}
}


// ------------------------------------------------------------------------------
//   Find all contours in the frame
// ------------------------------------------------------------------------------

vector<vector<Point> > Insect::findObjects(Mat* inputImage) 
{	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(*inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	
	return contours;
}


// ------------------------------------------------------------------------------
//   Map contours to their mean brightness
// ------------------------------------------------------------------------------

map<double, vector<Point> > Insect::mapContours(vector<vector<Point> > inputContours, Mat* inputImage)
 {
	map<double, vector<Point> > contourMap;

	for (int i = 0; i < inputContours.size(); i++)
	{
		// Create a mask image for each contour
		Mat mask = Mat::zeros(inputImage->size(), CV_8UC1);
		drawContours(mask, inputContours, i, cv::Scalar(255), CV_FILLED, CV_AA, noArray(), 1, Point(0, 0));

		// Map the brightness beneath the mask to the contour
		contourMap[mean(*inputImage, mask)[0]] = inputContours[i];
	}
	return contourMap;
}



// ------------------------------------------------------------------------------
//   Macro to combine all the steps necessary to find the insect
// ------------------------------------------------------------------------------

void Insect::findInsect(Mat* inputImage) 
{
	Mat values[3], lum;

	// Split into channels
	split(*inputImage, values);						
	lum = values[0];

	//Perform Dynamic thresholding on the saturation image
	int lumThreshold = findThreshold(lum);		
	
	// Check to ensure a valid threshold has been found
	if (lumThreshold < 0) 
	{
		found = false;
		return;
	}

	// Perform threshold on the image
	threshold(lum, lum, lumThreshold, 255, 0);

	// Find the contours in the image
	vector<vector<Point> > imageContours = findObjects(&lum);

	// If there are more than 20 contours you can be confident that threshold has failed
	if (imageContours.size() > 20 || imageContours.size() == 0) {
		found = false;
		return;
	}

	// Map contours to their brightnesses
	map<double, vector<Point> > contourMap = mapContours(imageContours, inputImage);
		
	// This takes advantage of the fact that maps are ordered by their keys, the first entry 
	// should be the brightest
	double insectIntensity = contourMap.rbegin()->first;

	// Find the centre of the insect from the moments
	Moments conMom = (moments(contourMap.rbegin()->second, false));
	Point insectCentre = Point2f(conMom.m10 / conMom.m00, conMom.m01 / conMom.m00);

	// Update the position
	updatePosition(insectCentre);
	found = true;

	return;
}


// ------------------------------------------------------------------------------
//   Get Height
// ------------------------------------------------------------------------------

int Insect::getHeight (void) 
{
	return heightBracket;
}

// ------------------------------------------------------------------------------
//   Get Velocity
// ------------------------------------------------------------------------------

Point2f Insect::getVelocity (void)
{
	return velocity;
}

// ------------------------------------------------------------------------------
//   Get Speed
// ------------------------------------------------------------------------------

float Insect::getSpeed (void)
{
	return speed;
}

// ------------------------------------------------------------------------------
//   Get Direction
// ------------------------------------------------------------------------------

float Insect::getDirection (void) 
{
	return direction;
}

// ------------------------------------------------------------------------------
//   Get Position
// ------------------------------------------------------------------------------

Point2f Insect::getPosition (void)
{
	return position;
}

// ------------------------------------------------------------------------------
//   Get Relative Position (from centre)
// ------------------------------------------------------------------------------

Point2f Insect::getRelPosition (void)
{
	return relPosition;
}

// ------------------------------------------------------------------------------
//   Get Relative Distance (from centre)
// ------------------------------------------------------------------------------

float Insect::getRelNorm (void)
{
	return relNorm;
}

// ------------------------------------------------------------------------------
//   Get Relative Angle (from centre)
// ------------------------------------------------------------------------------

float Insect::getRelAngle (void)
{
	return relAngle;
}

// ------------------------------------------------------------------------------
//   Return Found
// ------------------------------------------------------------------------------

bool Insect::isFound (void)
{
	return found;
}

// ------------------------------------------------------------------------------
//   Get ROI
// ------------------------------------------------------------------------------

Rect Insect::getROI (void)
{
	return ROI;
}



// ------------------------------------------------------------------------------
//   Output parameters to ncurses window
// ------------------------------------------------------------------------------

void Insect::printParameters(int y, int x)
{
	outputMap["Insect Data"] = 0;

	outputMap["Insect Found:"] = 2;
	outputMap["X Position:"] = 3;
	outputMap["Y Position:"] = 4;
	outputMap["Height:"] = 7;
	outputMap["Angle from Centre:"] = 6;
	outputMap["Distance from Centre:"] = 5;

	printDataWindow(outputMap, y, x);

}


// ------------------------------------------------------------------------------
//   Refresh data in ncurses window
// ------------------------------------------------------------------------------

void Insect::printOutput(void) {

	werase(output.insectData);
	mvwprintw(output.insectData, outputMap["Insect Found:"], 0, "%d\n", found);
	mvwprintw(output.insectData, outputMap["X Position:"], 0, "%.2f\n", relPosition.x);
	mvwprintw(output.insectData, outputMap["Y Position:"], 0, "%.2f\n", relPosition.y);
	mvwprintw(output.insectData, outputMap["Height:"], 0, "%d\n", heightBracket);
	mvwprintw(output.insectData, outputMap["Angle from Centre:"], 0, "%.2f\n", relAngle);
	mvwprintw(output.insectData, outputMap["Distance from Centre:"], 0, "%d\n", found);
}











