#ifndef FPS_H
#define FPS_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

double get_wall_time(void);
double get_cpu_time(void);

#endif