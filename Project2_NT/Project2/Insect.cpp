
#include "Insect.h"

#define HEIGHT_FILTER_SIZE 10
#define HEIGHT_BRACKET 20

Insect::Insect() {
	found = true;
	heightMA = vector<int>(HEIGHT_FILTER_SIZE, 0);
	position = Point(0, 0);
	prevPosition = position;
	speed = 0.0;
}


void Insect::updateHeight(int brightness) {
	heightMA.push_back(brightness);
	heightMA.erase(heightMA.begin());

	heightBracket = std::accumulate(heightMA.begin(), heightMA.end(), 0) / (HEIGHT_FILTER_SIZE * HEIGHT_BRACKET);
}

void Insect::updatePosition(Point2f centre, Rect ROI) {
	position = Point2f(centre.x + ROI.x, centre.y + ROI.y);
	speed = norm(position - prevPosition);
	prevPosition = position;
}