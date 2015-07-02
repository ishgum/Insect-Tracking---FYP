#include "beetle.h"


Beetle::Beetle() //constructor
{
	found = false;
	state.assign(6, 0); //x, y, z, dx, dy, dz = 0;
}


Beetle::~Beetle() //destructor
{
}
