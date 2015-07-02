#ifndef BEETLE_H
#define BEETLE_H

#include <vector>

class Beetle
{
public:
	Beetle();
	~Beetle();

	bool found;
	std::vector<float> state; //x, y, z, dx, dy, dz
	
};

#endif