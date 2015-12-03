
#ifndef THRESHOLDING
#define THRESHOLDING

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

//#define THRESH_GPU

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

#ifdef THRESH_GPU
	int findThreshold(GpuMat inputImage);
#else
	int findThreshold(Mat inputImage);
#endif

#endif
