#include "fps.h"
// Functions for system clock so we can determine runtine FPS
// copied from http://stackoverflow.com/questions/17432502
//  Windows
#ifdef _WIN32
	double get_wall_time(void){
		LARGE_INTEGER time, freq;
		if (!QueryPerformanceFrequency(&freq)){
			//  Handle error
			return 0;
		}
		if (!QueryPerformanceCounter(&time)){
			//  Handle error
			return 0;
		}
		return (double)time.QuadPart / freq.QuadPart;
	}
	double get_cpu_time(void){
		FILETIME a, b, c, d;
		if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0){
			//  Returns total user time.
			//  Can be tweaked to include kernel times as well.
			return
				(double)(d.dwLowDateTime |
				((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
		}
		else{
			//  Handle error
			return 0;
		}
	}
//  Posix/Linux
#else
	double get_wall_time(void){
		struct timeval time;
		if (gettimeofday(&time, NULL)){
			//  Handle error
			return 0;
		}
		return (double)time.tv_sec + (double)time.tv_usec * .000001;
	}
	double get_cpu_time(void){
		return (double)clock() / CLOCKS_PER_SEC;
	}
#endif
