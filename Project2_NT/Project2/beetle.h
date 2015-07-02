#ifndef BEETLE_H
#define BEETLE_H

#include <vector>

class beetle
{
public:
	beetle();
	~beetle();

	bool found;
	std::vector<float> state; //x, y, z, dx, dy, dz
	
};

#endif