/* Beetle class,
Only x & y implemented for now */

#ifndef BEETLE_H
#define BEETLE_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <math.h> 

using cv::Point;

class Beetle
{
public:
	// Methods
	Beetle();
	~Beetle();
	void updateState(int x, int y);		// update xy pos & vel
	Point posInFrame(void);				// return xy pos as a Point
	
	// Variables
	bool found;					 // true if bug found in last frame
	std::vector<float> state;	 // x, y, z, dx, dy, dz pixel co-ordinates
	float velocity;				 // norm of dx dy dz
};

#endif