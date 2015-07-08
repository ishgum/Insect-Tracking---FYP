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

float checkFPS(int wait_period){
	//init
	static int frame_num = 1;
	// Timing functions
	static double wall0 = get_wall_time();
	static double cpu0 = get_cpu_time();
	static int num_frames_proc = -1;
	static float fps_wall, fps_cpu = 0;
	static double cpu_running_total = 0;

	//Update
	num_frames_proc++;
	double wall1 = get_wall_time();
	double cpu1 = get_cpu_time();
	//cpu_running_total += (cpu1 - cpu0);

	//cpu0 = get_cpu_time();


	//printf("\t%f\t%f  ",wall1, wall1 - wall0);
	if (wall1 >= (wall0 + 1)){ //Check if 1s has elapsed
		//fps_wall_displayed = num_frames_proc / (wall1 - wall0); //rate displayed
		fps_wall = num_frames_proc / (wall1 - wall0 - num_frames_proc*0.001*wait_period);
		fps_cpu = num_frames_proc / (cpu1 - cpu0);// - num_frames_proc*0.001*wait_period);
		printf("\tFPS:  %f\t", fps_cpu);
		num_frames_proc = 0;
		wall0 = get_wall_time();
		cpu0 = get_cpu_time();
	}
	return fps_cpu;
}

void displayFPS(Mat src, Rect ROI, float fps_cpu){
	static int frame_num = 1;
	// Display FPS
	Mat src_w_text = src.clone();	// so we don't mess up original source
	rectangle(src_w_text, ROI, Scalar(255, 255, 255), 2, 8, 0);
	//resize(src_w_text, src_w_text, Size(), 0.3, 0.3);
	// add fps text to "1. Frame" window
	// Displayed FPS accounts for the delay we add in waitkey, far, far below.

	//wall time
	/*char fps_wall_c[4];
	sprintf(fps_wall_c, "wall FPS %.2f", fps_wall);
	Point fps_wall_text_loc(10, 30);
	putText(src_w_text, fps_wall_c, fps_wall_text_loc,
	FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);*/

	//cpu time
	char fps_cpu_c[14];
	sprintf(fps_cpu_c, "cpu FPS %4.2f", fps_cpu);
	Point fps_cpu_text_loc(10, 60);
	putText(src_w_text, fps_cpu_c, fps_cpu_text_loc, FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);

	//Frame Number
	char frame_num_c[15];
	sprintf(frame_num_c, "Frame #: %5i", frame_num);
	Point frame_num_text_loc(10, 90);
	putText(src_w_text, frame_num_c, frame_num_text_loc, FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);
	imshow("FPS", src_w_text);

	//printf("\nFrame: %i\tCPU total: %f\tCPU FPS: %f", frame_num, cpu_running_total, fps_cpu);
	frame_num++;

	//#ifdef RECORD_SOURCE_W_BOX
	//		// write output video w/ text
	//		outputVideo.write(src_w_text);
	//#endif
}