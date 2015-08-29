
#ifndef THRESHOLDING
#define THRESHOLDING

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

//#define THRESH_GPU

#define THRESHOLD_LEVEL 10

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
	map<int, int> lobeMap;
	map<int, int> maxMap;

public:


	myHist();
	myHist(Mat);
	void findPeaksandMins(int);
	int findThresholdByArea(int);
	void findLobes(int);
	int findThresholdByLobes();
};

#ifdef THRESH_GPU
	int findThreshold(GpuMat inputImage);
#else
	int findThreshold(Mat inputImage);
#endif

#endif
