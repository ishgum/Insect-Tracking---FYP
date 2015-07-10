#include "fps.h"

// Functions for system clock so we can determine runtine FPS
// copied from http://stackoverflow.com/questions/17432502
//  Windows
#ifdef _WIN32
double Fps::get_wall_time(void){
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
double Fps::get_cpu_time(void){
	FILETIME a, b, c, d; // see https://msdn.microsoft.com/en-us/library/windows/desktop/ms724284(v=vs.85).aspx for deets
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0){
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			//(double)(d.dwLowDateTime)*0.0000001; // return time in s
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
double Fps::get_wall_time(void){
	struct timeval time;
	if (gettimeofday(&time, NULL)){
		//  Handle error
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double Fps::get_cpu_time(void){
	return (double)clock() / CLOCKS_PER_SEC;
}
#endif

Fps::Fps(int wait_period, fps_mode_t mode) {
	mode_ = mode;
	frame_num = 1;
	wall0 = get_wall_time();
	cpu0 = get_cpu_time();
	num_frames_proc = 0;
	fps_wall = 0, fps_cpu = 0;
	cpu_running_total = 0;
	wait_period_ = wait_period;
	wall1 = wall0;
	cpu1 = cpu0;	
}

void Fps::checkFPS(void){
	frame_num++;
	num_frames_proc++;

	switch (mode_){
	case (fps_mode_t::WALL):
		wall1 = get_wall_time();
		if (wall1 >= (wall0 + 1)){ // check if >1s has elapsed
			fps_wall = num_frames_proc / (wall1 - wall0 - num_frames_proc*0.001*wait_period_);
			printf("\tWALL:\t%f\t", fps_wall);
			num_frames_proc = 0;
			wall0 = get_wall_time();
		}
		break;
	case(fps_mode_t::CPU) :
		cpu1 = get_cpu_time();
		if ((cpu1 > cpu0)){ // check if reported cpu time has changed. sometimes get_cpu_time appears not to change for an unknown reason

			fps_cpu = num_frames_proc / (cpu1 - cpu0);// - num_frames_proc*0.001*wait_period);
			printf("\tFPS:  %f\t", fps_cpu);
			num_frames_proc = 0;
			cpu_running_total += (cpu1 - cpu0);
			cpu0 = get_cpu_time();
		}
		break;
	case(fps_mode_t::BOTH) :
		wall1 = get_wall_time();
		cpu1 = get_cpu_time();
		if (wall1 >= (wall0 + 1) && (cpu1 > cpu0)){ //Check if >1s has elapsed And,
			// check if reported cpu time has changed. sometimes get_cpu_time appears not to change for an unknown reason

			//fps_wall_displayed = num_frames_proc / (wall1 - wall0); //rate displayed
			fps_wall = num_frames_proc / (wall1 - wall0 - num_frames_proc*0.001*wait_period_);
			fps_cpu = num_frames_proc / (cpu1 - cpu0);// - num_frames_proc*0.001*wait_period);
			printf("\tFPS:  %f\tWALL:\t%f\t", fps_cpu, fps_wall);
			num_frames_proc = 0;
			cpu_running_total += (cpu1 - cpu0);
			wall0 = get_wall_time();
			cpu0 = get_cpu_time();
		}
		break;
	}

}

void Fps::displayFPS(Mat src, Rect ROI){
	// Display FPS
	Mat src_w_text = src.clone();	// so we don't mess up original source
	rectangle(src_w_text, ROI, Scalar(255, 255, 255), 2, 8, 0);
	//resize(src_w_text, src_w_text, Size(), 0.3, 0.3);
	// add fps text to "1. Frame" window
	// Displayed FPS accounts for the delay we add in waitkey, far, far below.

	Point fps_wall_text_loc(10, 30);
	Point fps_cpu_text_loc(10, 60);
	Point frame_num_text_loc(10, 90);
	char fps_wall_c[30];  // Needs to be large enough to avoid corrupting stack
	char fps_cpu_c[30];   // Needs to be large enough to avoid corrupting stack
	char frame_num_c[30]; // Needs to be large enough to avoid corrupting stack


	if (mode_ == fps_mode_t::WALL || mode_ == fps_mode_t::BOTH){
		//wall time
		sprintf(fps_wall_c, "wall FPS %4.2f", fps_wall);
		putText(src_w_text, fps_wall_c, fps_wall_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);
	}
	if (mode_ == fps_mode_t::CPU || mode_ == fps_mode_t::BOTH){
		//cpu time
		sprintf(fps_cpu_c, "cpu FPS %4.2f", fps_cpu);
		putText(src_w_text, fps_cpu_c, fps_cpu_text_loc,
			FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);
	}

	//Frame Number
	sprintf(frame_num_c, "Frame #: %5i", frame_num);
	putText(src_w_text, frame_num_c, frame_num_text_loc, FONT_HERSHEY_SIMPLEX, 0.6, { 255, 255, 255 }, 1.5);
	imshow("FPS", src_w_text);

	//printf("\nFrame: %i\tCPU total: %f\tCPU FPS: %f", frame_num, cpu_running_total, fps_cpu);
	frame_num++;

	//#ifdef RECORD_SOURCE_W_BOX
	//		// write output video w/ text
	//		outputVideo.write(src_w_text);
	//#endif
}