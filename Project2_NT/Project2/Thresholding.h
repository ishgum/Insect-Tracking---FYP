
#ifndef THRESHOLDING
#define THRESHOLDING

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace cv;
using namespace std;
using namespace cv::gpu;

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


int findThreshold(GpuMat inputImage);

#endif
