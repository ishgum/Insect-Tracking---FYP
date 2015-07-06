
#ifndef THRESHOLDING
#define THRESHOLDING

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;


class myHist {
	vector<double> histogram;
	vector<int> peakLocs;
	vector<int> minLocs;
	int absMaxLoc;
	bool checkThreshold(int);
	int findNextMin(int);
public:
	int threshold;

	myHist();
	myHist(Mat);
	void findPeaksandMins(int);
	void findThresholdByArea(int);
};


int findThreshold(Mat inputImage);

#endif