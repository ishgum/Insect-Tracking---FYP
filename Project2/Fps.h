/* disabled CPU fps functionality
get_cpu_time on windows is not reliable
see readme*/

#ifndef FPS_H
#define FPS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
using namespace cv;

#ifdef _WIN32
#include <Windows.h>
#else
// UNIX support
#include <time.h>
#include <sys/time.h>
#endif

enum class fps_mode_t {WALL, CPU, BOTH};

class Fps {
public:
	Fps(int wait_period, fps_mode_t mode);
	void checkFPS(void);
	void displayFPS(Mat src, Rect ROI);

	fps_mode_t mode_;
	int frame_num;
	int num_frames_proc;
	float fps_wall, fps_cpu;
	double cpu_running_total;
	double get_wall_time(void);
	double get_cpu_time(void);
private:


	double wall1;
	double cpu1;
	int wait_period_;
	double wall0;
	double cpu0;
};



#endif