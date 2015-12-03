#ifndef INSECT
#define INSECT

#include <opencv2/core/core.hpp>
#include <numeric> 

using namespace cv;
using namespace std;


#define ROI_SIZE .15
#define HEIGHT_OFFSET 10

class Insect {
	vector<int> heightMA;
	Point2f prevPosition;
	Point2f frameCentre;

	void humanReadableOutput(void);
public:
	int heightBracket;
	Point2f velocity;
	float speed;
	float direction;
	Point2f position;
	Point2f relPosition;
	float relNorm;
	float relAngle;
	bool found;
	Rect ROI;

	Insect(Mat*);
	void updateHeight(int);
	void updatePosition(Point2f);
	void updateROI(Mat*);
};

#endif // !INSECT
