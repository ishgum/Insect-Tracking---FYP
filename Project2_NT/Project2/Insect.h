#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

using namespace std;
using namespace cv;


class Insect {
	bool found;
	int threshold;
	Point position, prevPosition;
public:
	void updatePosition(Point);
	Insect();
};