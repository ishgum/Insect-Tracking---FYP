#ifndef INSECT
#define INSECT


#include <opencv2/core/core.hpp>
#include <numeric> 




using namespace cv;
using namespace std;

class Insect {
	vector<int> heightMA;
	Point2f prevPosition;

public:
	int heightBracket;
	double speed;
	Point2f position;
	bool found;

	Insect();
	void updateHeight(int);
	void updatePosition(Point2f, Rect);
};

#endif // !INSECT