/*

*/
#include "main.h"


/********** @function main ***********/
int main(int argc, char** argv)
{

	VideoCapture capture;

	/********** SOURCE VIDEO ************/

	//EARLY TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Ancient_times/plainHigh1.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2990.MOV"); //runs at ~6fps
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/Tests/MVI_2987.MOV");

	//IR RREFLEC TESTS:
	//capture.open("C:/Users/myadmin/Documents/IR footage/retro2_2015-05-09-193310-0000.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only.avi"); 
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193310-0000_8seconds_only_Uncompressed_Grayscale.avi");
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro2_2015-05-09-193006-0000_8bit_uncompressed.avi"); // Princess Beetle and the sparkly dress, Co-Staring Michael
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR footage/retro1_2015-05-09-192708-0000.avi"); //persistent bright region on lower portion of frame

	//DEPTH TESTS:
	//capture.open("C:/Users/myadmin/Documents/_M2D2/Data/IR_footage_depth/realRun2_0.avi");
	
	// FOR BASIC CODE TEST. uses a relative path which is handy, but could break
	capture.open("../../test.avi");		//File is greyscale, and dim NOT square

	//DYLANS folder structure:
	//capture.open("C:/Users/Dylan/Documents/FYP/data/MVI_2987.MOV");

	/********** END SOURCE VIDEO ************/


	int usable_contours = 0;

	#ifdef RECORD_SOURCE_W_BOX
		int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		int input_fps = capture.get(CV_CAP_PROP_FPS);
		input_fps = 25;
		VideoWriter outputVideo("../../../Output/out.avi", CV_FOURCC('M', 'J', 'P', 'G'), input_fps, Size(frame_width, frame_height), true);
	#endif

	#ifdef KALMAN
		KalmanFilter KF(4, 2, 0);
		Mat state(4, 1, CV_32F); //x, y, delta x, delta y
		Mat processNoise(4, 1, CV_32F);
		KF = setKalmanParameters(KF);
	#endif

	Point xy_loc(capture.get(CV_CAP_PROP_FRAME_WIDTH) / 2, capture.get(CV_CAP_PROP_FRAME_HEIGHT / 2));
	
	int kalmanCount = 0;
	Mat src; Mat src_ROI;
	Point prevCentre;

	capture >> src;
	//resize(src, src, Size(), 0.3, 0.3);
	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame
	float cpuFPS = 0;
	vector<Point2f> mc;

/********** WHILE LOOP ***********/
	while (!src.empty()) {

		#ifdef RECORD_SOURCE_W_BOX
			sourceDisplayAndRecord(src, ROI, outputVideo);
		#endif

		#ifdef FPS
			cpuFPS = checkFPS(WAIT_PERIOD);
		#endif

		#if (DEBUG ==1)
			displayFPS(src, ROI, cpuFPS);
		#endif

		// processFrame
		mc = processFrame(src, ROI, noBug, threshFilter, THRESH_FILTER_SIZE, xy_loc,
			usable_contours, DEBUG, RGB_SOURCE);
	
		// KALMAN
		#ifdef KALMAN
			useKalmanFilter(KF, xy_loc, usable_contours, src, ROI, mc, DEBUG);
		#endif

		Point contourCentre;
		Point centreDiff;

		if (noBug == true) {
			kalmanCount = 0;
		}
		else {
			contourCentre = Point(mc[0].x + ROI.x, mc[0].y + ROI.y);
		}

		centreDiff = contourCentre - prevCentre;
		float value = norm(centreDiff);
		printf("Speed: %.1f	", value);
		//printf("Just trying: %i", (value + threshSum / THRESH_FILTER_SIZE));
		prevCentre = contourCentre;

		ROI.x = 0;
		ROI.y = HEIGHT_OFFSET;		//Strange artifacts in top left hand corner removed
		ROI.width = src.cols;
		ROI.height = src.rows-HEIGHT_OFFSET;

		if (kalmanCount++ > 10) { // if bug is lost, wait until we have it for 10 frames before ROI update

			ROI = updateROI(ROI, contourCentre, src, ROI_SIZE, noBug);
		}
		capture >> src;
		//resize(src, src, Size(), 0.3, 0.3);
		waitKey(WAIT_PERIOD); 
		printf("\n");
	}
	capture.release();
	cout << "Done\n";

	return(0);
}