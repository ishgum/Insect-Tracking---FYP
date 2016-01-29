
#include "ui_output.h"


using namespace std;




OutputStream::OutputStream(void) {

}


void OutputStream::init(void) {

	outputStream = newwin(OUTPUT_STREAM_HEIGHT, OUTPUT_STREAM_WIDTH, OUTPUT_STREAM_Y, OUTPUT_STREAM_X);
	
	scrollok(outputStream, TRUE);


	insectData = newwin(8, DATA_WIDTH, INSECT_DATA_Y, DATA_COLUMN);
	uavData = newwin(6, DATA_WIDTH, UAV_DATA_Y, DATA_COLUMN);
	cameraData = newwin(5, DATA_WIDTH, CAMERA_DATA_Y, DATA_COLUMN);
	flagData = newwin(6, DATA_WIDTH, FLAG_DATA_Y, DATA_COLUMN);

}

void OutputStream::print(const char* s) {
	wprintw(outputStream, "%s", s);
}


void OutputStream::refresh(void) {
	wrefresh(outputStream);
	wrefresh(insectData);
	wrefresh(uavData);
	wrefresh(cameraData);
	wrefresh(flagData);
}



OutputStream output;








void printDataWindow(map<const char*, int> in_map, int y, int x)
{
	map<const char*,int>::iterator it;
	int width = 0;
	
	for (it=in_map.begin(); it!=in_map.end(); ++it)
	{	
        int wordLength = strlen(it->first);
        if (wordLength > width) width = wordLength;
	}


	WINDOW * window = newwin(in_map.size()+1, width, y, x);



	for (it=in_map.begin(); it!=in_map.end(); ++it)
	{
        if (it->second == 0) wattron(window, A_UNDERLINE);  
  
 		mvwprintw(window, it->second, width - strlen(it->first), it->first);

        if (it->second == 0) wattroff(window, A_UNDERLINE);
	}

    wrefresh(window);
}





void printMainScreen(void) {
	int h, w;
	getmaxyx(stdscr, h, w);


	attron(A_STANDOUT);
	mvprintw(0, 0, " Insect Tracking Program");
	chgat(-1, A_STANDOUT, 0, NULL);
	
	
	mvchgat(h-2, 0, -1, A_STANDOUT, 0, NULL);
	mvchgat(h-1, 0, -1, A_STANDOUT, 0, NULL);
	
    mvprintw(h-2, 2, "q: Quit");	
    mvprintw(h-2, 15, "c: Connect Camera");
    mvprintw(h-2, 35, "r: Refresh screen");
    mvprintw(h-2, 60, "d: View ROI");
    mvprintw(h-1, 2, "p: Pause");
    mvprintw(h-1, 15, "u: UAV Control");
    mvprintw(h-1, 35, "e: Check response");
    mvprintw(h-1, 60, "f: View full frame");
    
	mvprintw(5, 49, "Output Stream");
	
	WINDOW * outputBox = newwin(OUTPUT_STREAM_HEIGHT+2, OUTPUT_STREAM_WIDTH+2, OUTPUT_STREAM_Y-1, OUTPUT_STREAM_X-1);
	box(outputBox, 0, 0);
	
    refresh();
	wrefresh(outputBox);


}
