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


void Insect::updateROI(Mat* src) {
	if (found) {
		// Updates the size and location of the region of interest
		int roiSize = ROI_SIZE * src->rows;

		if (position.x > (roiSize / 2)) {
			ROI.x = int(position.x) - roiSize / 2;
		}
		if (position.y > (roiSize / 2)) {
			ROI.y = int(position.y) - roiSize / 2;
		}
		if (position.x + roiSize / 2 > src->cols) {
			ROI.x = src->cols - roiSize;
		}
		if (position.y + roiSize / 2 > src->rows) {
			ROI.y = src->rows - roiSize;
		}
		ROI.width = roiSize;
		ROI.height = roiSize;
	}
	else {
		ROI = Rect(0, HEIGHT_OFFSET, src->cols, src->rows - HEIGHT_OFFSET); //Reset ROI
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
