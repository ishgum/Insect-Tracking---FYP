/*

*/
#include "main.h"

void sourceDisplayAndRecord(Mat src, Rect ROI, VideoWriter outputVideo){
	Mat srcBox = src.clone();
	rectangle(srcBox, ROI, Scalar(255, 255, 255), 2, 8, 0);
	imshow("Source w Box", srcBox);
	outputVideo.write(srcBox); //write output video w/o fps text

}

void drawCross(Mat img, Point centre, Scalar colour, int d)
{
	line(img, Point(centre.x - d, centre.y - d), Point(centre.x + d, centre.y + d), colour, 2, CV_AA, 0);
	line(img, Point(centre.x + d, centre.y - d), Point(centre.x - d, centre.y + d), colour, 2, CV_AA, 0);
}

Rect updateROI(Rect ROI, Point stateLoc, Mat src) {
	int roiSize = ROI_SIZE * src.rows;

	if (noBug == false) {

		if (stateLoc.x > (roiSize / 2)) {
			ROI.x = stateLoc.x - roiSize / 2;
		}
		if (stateLoc.y > (roiSize / 2)) {
			ROI.y = stateLoc.y - roiSize / 2;
		}
		if (stateLoc.x + roiSize / 2 > src.cols) {
			ROI.x = src.cols - roiSize;
		}
		if (stateLoc.y + roiSize / 2 > src.rows) {
			ROI.y = src.rows - roiSize;
		}
		ROI.width = roiSize;
		ROI.height = roiSize;
	}
	return ROI;
}

/********** @function main ***********/
int main(int argc, char** argv)
{
	Mat measurement = Mat::zeros(2, 1, CV_32F);
	int usable_contours = 0;

	VideoCapture capture;

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


	//#ifdef RECORD_SOURCE_W_BOX
		int frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		int input_fps = capture.get(CV_CAP_PROP_FPS);
		input_fps = 25;
		VideoWriter outputVideo("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), input_fps, Size(frame_width, frame_height), true);
	//#endif

	#ifdef KALMAN
		KalmanFilter KF(4, 2, 0);
		Mat state(4, 1, CV_32F); //x, y, delta x, delta y
		Mat processNoise(4, 1, CV_32F);
		KF = setKalmanParameters(KF);
		vector<Point> targetv, kalmanv;
		//Point xy_loc(capture.get(CV_CAP_PROP_FRAME_WIDTH) / 2, capture.get(CV_CAP_PROP_FRAME_HEIGHT/2));
	#endif
	Point xy_loc(capture.get(CV_CAP_PROP_FRAME_WIDTH) / 2, capture.get(CV_CAP_PROP_FRAME_HEIGHT / 2));
	
	int kalmanCount = 0;
	Mat src; Mat src_ROI;
	Point prevCentre;

	capture >> src;
	//resize(src, src, Size(), 0.3, 0.3);
	Rect ROI(0, 0, src.cols, src.rows); // Set ROI to whole image for first frame
	float cpuFPS = 0;

/********** WHILE LOOP ***********/
	while (!src.empty()) {

		sourceDisplayAndRecord(src, ROI, outputVideo);
		cpuFPS = checkFPS(WAIT_PERIOD);
		displayFPS(src, ROI, cpuFPS);



		// processFrame
		//processFrame(src, ROI, noBug, threshFilter, THRESH_FILTER_SIZE, xy_loc, usable_contours)
		src_ROI = src(ROI);

		#ifdef DEBUG
			imshow("Frame", src_ROI);
		#endif // DEBUG

		Mat dst = preprocessImage(src_ROI, noBug, threshFilter, THRESH_FILTER_SIZE);
		
		Canny(dst, dst, 100, 100 * 2, 3);

		vector<Point2f> mc;
		mc = contourProcessing(dst, ROI, xy_loc, usable_contours, noBug);
		// end processFrame
	
		#ifdef KALMAN
			//Prediction
			Mat predict = KF.predict();
			Point xy_loc(predict.at<float>(0), predict.at<float>(1));
			Point xy_vel(predict.at<float>(2), predict.at<float>(3));

			//Attempt to allow tracking of vanishing target
			KF.statePre.copyTo(KF.statePost);
			KF.errorCovPre.copyTo(KF.errorCovPost);

			//Get measurements
			if (usable_contours >= 1) {	// found contours, use contour centres
				measurement.at<float>(0) = mc[0].x + float(ROI.x);
				measurement.at<float>(1) = mc[0].y + float(ROI.y);
			}
			else {						//didn't find anything, use prediction
				measurement.at<float>(0) = xy_loc.x;
				measurement.at<float>(1) = xy_loc.y;
			}

			//Update filter
			Mat correction = KF.correct(measurement);
			Point stateLoc(correction.at<float>(0), correction.at<float>(1));
	
			Point stateVel(correction.at<float>(2), correction.at<float>(3));
			Point measLoc(measurement.at<float>(0), measurement.at<float>(1));
			targetv.push_back(measLoc);
			kalmanv.push_back(stateLoc);

			#ifdef DEBUG
				// plot stuff
				src = Scalar::all(0);
				drawCross(src, stateLoc, Scalar(255, 255, 255), 5);
				drawCross(src, measLoc, Scalar(0, 0, 255), 5);

				for (int i = 0; i < targetv.size() - 1; i++)
					line(src, targetv[i], targetv[i + 1], Scalar(255, 255, 0), 1);

				for (int i = 0; i < kalmanv.size() - 1; i++)
					line(src, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

				imshow("Frame Kalman", src);
			#endif	//DEBUG
		#endif //Kalman
		

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

		if (kalmanCount++ > 10) {

			ROI = updateROI(ROI, contourCentre, src);
		}

		capture >> src;
		//resize(src, src, Size(), 0.3, 0.3);

		waitKey(WAIT_PERIOD); 
		printf("\n");
	}

	capture.release();

	//while (waitKey(10) < 0) {
		cout << "Done\n";
	//}

	return(0);
}