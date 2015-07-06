#include "Insect.h"


Insect::Insect() {
	found = false;
	threshold = 0;
	position = Point(0, 0);
	prevPosition = position;
}



void Insect::updatePosition(Point currentPosition) {
	prevPosition = position;
	position = currentPosition;
}