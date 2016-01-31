/****************************************************************************
 *
 *
 * @file insect.h
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



#ifndef INSECT_H_
#define INSECT_H_


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <numeric> 



#include "threshold.h"
#include "common.h"


// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace cv;
using namespace std;



// ------------------------------------------------------------------------------
//   Defines
// ------------------------------------------------------------------------------

#define ROI_SIZE .15
#define HEIGHT_OFFSET 10
#define WIDTH_OFFSET 10


#define HEIGHT_FILTER_SIZE 10
#define HEIGHT_BRACKET 20


// ----------------------------------------------------------------------------------
//   Insect Class
// ----------------------------------------------------------------------------------
/*
 * Insect Class
 *
 * This provides functions to store the data for the current pose of the insect,
 * provides functions necessary to find the current pose of the insect and also
 * gives the ability to print that data to an ncurses window.
 * 
 */


class Insect 
{

private:

	vector<vector<Point> > findObjects(Mat*);
	map<double, vector<Point> > mapContours(vector<vector<Point> > inputContours, Mat* inputImage);

	vector<int> heightMA;
	map<const char*, int> outputMap;

	int heightBracket;

	float relNorm;
	float relAngle;
	float speed;
	float direction;

	Point2f velocity;
	Point2f position;
	Point2f relPosition;
	Point2f prevPosition;
	Point2f frameCentre;

	bool found;

	Size size;
	Rect ROI;

public:

	// Contructors
	Insect(Size size);

	// Setters
	void updateHeight(int);
	void updatePosition(Point2f);
	void updateROI();

	// Commands
	void findInsect(Mat* inputImage);
	
	// Getters
	int getHeight();
	Point2f getVelocity();
	float getSpeed();
	float getDirection();
	Point2f getPosition();
	Point2f getRelPosition();
	float getRelNorm();
	float getRelAngle();
	bool isFound();
	Rect getROI();

	// Output
	void printParameters(int, int);
	void printOutput(void);

};

#endif // !INSECT_H_
