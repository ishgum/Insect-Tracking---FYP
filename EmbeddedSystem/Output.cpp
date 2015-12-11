
#include "Output.h"


using namespace std;


/*
menu::menu(int x, int y, const char* name) {
	width = strlen(name);
	rows = 0;
	x_pos = x;
	y_pos = y;

	title = name;

}


void menu::create(void) {
	width = lobeMap.rbegin()->second;
	window = newwin(rows + 2, width, y_pos, x_pos);

	wattron(window, A_UNDERLINE);
    mvwprintw(window, 0, width, title);
    wattroff(window, A_UNDERLINE);
	
}




void menu::addRow(const char* rowText)
{
	entryMap[rowText] = strlen(rowText);
}
*/


OutputStream::OutputStream(void) {

}


void OutputStream::init(void) {
	int h, w;
    getmaxyx(stdscr, h, w);
	outputStream = newwin(h-20, 38, 7, 50);
	scrollok(outputStream, TRUE);


	insectData = newwin(6, 20, 7, 27);
	uavData = newwin(4, 20, 17, 27);
	cameraData = newwin(3, 20, 25, 27);
	flagData = newwin(4, 20, 32,27);

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


void printInsectWindow(int height, int width, int y, int x)
{
	WINDOW * window = newwin(height, width, y, x);

    wattron(window, A_UNDERLINE);
    mvwprintw(window, 0, width - 11, "Insect Data");
    wattroff(window, A_UNDERLINE);

    mvwprintw(window, 2, width - 13, "Insect Found:");
    mvwprintw(window, 3, width - 11, "X Position:");
    mvwprintw(window, 4, width - 11, "Y Position:");
    mvwprintw(window, 5, width - 7, "Height:");
    mvwprintw(window, 6, width - 18, "Angle from Centre:");
    mvwprintw(window, 7, width - 21, "Distance from Centre:");

    wrefresh(window);
}

void printUAVWindow(int height, int width, int y, int x)
{
	WINDOW * window = newwin(height, width, y, x);

    wattron(window, A_UNDERLINE);
    mvwprintw(window, 0, width - 8, "UAV Data");
    wattroff(window, A_UNDERLINE);

    mvwprintw(window, 2, width - 14, "UAV Connected:");
    mvwprintw(window, 3, width - 11, "X Position:");
    mvwprintw(window, 4, width - 11, "Y Position:");
    mvwprintw(window, 5, width - 11, "Z Position:");


    wrefresh(window);
}


void printCameraWindow(int height, int width, int y, int x)
{
	WINDOW * window = newwin(height, width, y, x);

    wattron(window, A_UNDERLINE);
    mvwprintw(window, 0, width - 11, "Camera Info");
    wattroff(window, A_UNDERLINE);

    mvwprintw(window, 2, width - 17, "Camera Connected:");
    mvwprintw(window, 3, width - 4, "FPS:");
	mvwprintw(window, 4, width - 11, "Frame Size:");


    wrefresh(window);
}


void printFlagWindow(int height, int width, int y, int x)
{
	WINDOW * window = newwin(height, width, y, x);

    wattron(window, A_UNDERLINE);
    mvwprintw(window, 0, width - 9, "Flag Info");
    wattroff(window, A_UNDERLINE);

    mvwprintw(window, 2, width - 10, "ROI Debug:");
    mvwprintw(window, 3, width - 17, "Full Frame Debug:");
	mvwprintw(window, 4, width - 7, "Paused:");
	mvwprintw(window, 5, width - 8, "Running:");


    wrefresh(window);
}






void printMainScreen(void) {
    int h, w;
   	getmaxyx(stdscr, h, w);


    attron(A_STANDOUT);
    mvprintw(0, 0, " Insect Tracking Program");
    chgat(-1, A_STANDOUT, 0, NULL);

    mvprintw(h-1,0, "Press 'q' to quit - Press 'p' to pause");

	mvprintw(5, 49, "Output Stream");
	WINDOW * outputBox = newwin(h-18, 40, 6, 49);
	box(outputBox, 0, 0);
	
    refresh();
	wrefresh(outputBox);

    printInsectWindow(8, 21, 5, 2);
	printUAVWindow(6, 14, 15, 9);
	printCameraWindow(5, 17, 23, 6);
	printFlagWindow(6, 17, 30, 6);

}
