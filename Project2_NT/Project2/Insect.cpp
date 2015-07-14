#include "Insect.h"

#define HEIGHT_FILTER_SIZE 10
#define HEIGHT_BRACKET 20

Insect::Insect(Mat* src) {
	found = false;
	heightMA = vector<int>(HEIGHT_FILTER_SIZE, 0);
	position = Point(0, 0);
	prevPosition = position;
	speed = 0.0;
	ROI = Rect(0, 0, src->cols, src->rows);
	frameCentre = Point(src->cols / 2, src->rows / 2);
}


void Insect::updateHeight(int brightness) {
	heightMA.push_back(brightness);
	heightMA.erase(heightMA.begin());

	heightBracket = std::accumulate(heightMA.begin(), heightMA.end(), 0) / (HEIGHT_FILTER_SIZE * HEIGHT_BRACKET);
}

void Insect::updatePosition(Point2f centre) {
	position = Point2f(centre.x + ROI.x, centre.y + ROI.y);
	
	velocity = position - prevPosition;
	speed = norm(velocity);
	direction = atan2f(velocity.y, velocity.x) * 180 / CV_PI;

	relPosition = position - frameCentre;
	relNorm = norm(relPosition);
	relAngle = atan2f(relPosition.y, relPosition.x) *180 / CV_PI;

	humanReadableOutput();
	prevPosition = position;
}


void Insect::updateROI(void ) {
	int srcSize = 2048;
	if (found) {
		// Updates the size and location of the region of interest
		int roiSize = srcSize / 8;

		if (position.x > (roiSize / 2)) {
			ROI.x = int(position.x) - roiSize / 2;
		}
		if (position.y > (roiSize / 2)) {
			ROI.y = int(position.y) - roiSize / 2;
		}
		if (position.x + roiSize / 2 > srcSize) {
			ROI.x = srcSize - roiSize - 1 ;
		}
		if (position.y + roiSize / 2 > srcSize) {
			ROI.y = srcSize - roiSize - 1;
		}
		if (ROI.x % 2) { ROI.x -= 1; }
		if (ROI.y % 2) { ROI.y -= 1; }

		ROI.width = roiSize;
		ROI.height = roiSize;
	}
	else {
		ROI = Rect(0, 0, srcSize, srcSize); //Reset ROI
	}
}

void Insect::humanReadableOutput(void) {
	printf("\n");
	printf("Distance: %.0f	", relNorm / 40);
	if (relAngle > 22.5 &&  relAngle < 157.5) {
		printf("Down ");
	}
	if (relAngle < -22.5 &&  relAngle > -157.5) {
		printf("Up ");
	}
	if (relAngle < -112.5 ||  relAngle > 112.5) {
		printf("Left ");
	}
	if (relAngle > -67.5 &&  relAngle < 67.5) {
		printf("Right ");
	}
}