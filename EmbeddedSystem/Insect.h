#ifndef INSECT
#define INSECT

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <numeric> 
#include <iostream>
#include <stdio.h>

#include "Threshold.h"

using namespace cv;
using namespace std;


#define ROI_SIZE .15
#define HEIGHT_OFFSET 10
#define WIDTH_OFFSET 10

class Insect {
	vector<int> heightMA;
	Point2f prevPosition;
	Point2f frameCentre;

	void humanReadableOutput(void);
public:
	int heightBracket;
	Point2f velocity;
	float speed;
	float direction;
	Point2f position;
	Point2f relPosition;
	float relNorm;
	float relAngle;
	bool found;
	Rect ROI;

	Insect(Mat*);
	void updateHeight(int);
	void updatePosition(Point2f);
	void updateROI(Mat*);
	vector<vector<Point> > findObjects(Mat*);
	map<double, vector<Point> > mapContours(vector<vector<Point> > inputContours, Mat* inputImage);
	void findInsect(Mat* inputImage);

};

#endif // !INSECT