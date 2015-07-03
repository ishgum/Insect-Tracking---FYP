#include "beetle.h"


Beetle::Beetle() //constructor
{
	found = false;
	state.assign(6, 0); //x, y, z, dx, dy, dz = 0;
}


Beetle::~Beetle() //destructor
{
}

void Beetle::updateState(int x, int y)
{
	// velocity
	state[3] = x - state[0];
	state[4] = y - state[1];
	velocity = sqrt(pow(state[3], 2) + pow(state[4], 2));

	// position
	state[0] = x;
	state[1] = y;
}

Point Beetle::posInFrame(void){
	return Point(state[0], state[1]);
}
