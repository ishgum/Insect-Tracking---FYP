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

double get_wall_time(void);
double get_cpu_time(void);
float checkFPS(int wait_period);
void displayFPS(Mat src, Rect ROI, float fps_cpu);

#endif