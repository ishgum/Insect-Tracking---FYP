#include "beetle.h"


beetle::beetle() //constructor
{
	found = false;
	state.assign(6, 0); //x, y, z, dx, dy, dz = 0;
}


beetle::~beetle() //destructor
{
}
