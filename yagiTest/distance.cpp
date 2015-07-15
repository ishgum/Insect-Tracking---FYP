// Function related to determining distance of the bug to the yagis
// 
// 

#include "distance.h"

/* Returns the minimum distance to the bug based on LUT
note: the in array should have increasing values
Modified from http://playground.arduino.cc/Main/MultiMap*/
float lookup_distance(float val, float * _in, int * _out, uint8_t size)
{
	// take care the value is within range
	// val = constrain(val, _in[0], _in[size-1]);
	if (val <= _in[0]) return _out[0];
	if (val >= _in[size - 1]) return _out[size - 1];

	// search right interval
	uint8_t pos = 1;  // _in[0] already tested
	while (val > _in[pos]) pos++;

	// this will handle all exact "points" in the _in array
	if (val == _in[pos]) return _out[pos];

	// return minimum distance away. e.g if 15m away, return 10
	return _out[pos];
}