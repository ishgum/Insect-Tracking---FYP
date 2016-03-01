/****************************************************************************
 *
 *
 * @file ui_output.cpp
 *
 * @brief Insect class which contains all of the data and methods for locating the insect
 *
 * Functions for finding the insect from an image as well as for encapsulating data for the
 * insect that is found.
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include "include/ui_output.h"


// ----------------------------------------------------------------------------------
//   Output Stream
// ----------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Constructor
// ------------------------------------------------------------------------------

OutputStream::OutputStream(void) {

}



// ------------------------------------------------------------------------------
//   Methods
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Initialise all of the output windows
// ------------------------------------------------------------------------------


void OutputStream::init(void) {

	// Set up for ncurses environment
	initscr();
	cbreak(); 		// Doesn't wait for the return key
	noecho();		// Doesn't print user input to screen
	curs_set(0);		// Makes the cursor invisible

	// Output stream is intended to act as a feedback to the user and can be used for debugging purposes
	outputStream = newwin(OUTPUT_STREAM_HEIGHT, OUTPUT_STREAM_WIDTH, OUTPUT_STREAM_Y, OUTPUT_STREAM_X);
	
	// Creates the data window
	insectData = newwin(8, DATA_WIDTH, INSECT_DATA_Y, DATA_COLUMN);
	uavData = newwin(6, DATA_WIDTH, UAV_DATA_Y, DATA_COLUMN);
	cameraData = newwin(5, DATA_WIDTH, CAMERA_DATA_Y, DATA_COLUMN);
	flagData = newwin(6, DATA_WIDTH, FLAG_DATA_Y, DATA_COLUMN);
	
	// Means that the output stream will scroll up when the text gets to the bottom
	scrollok(outputStream, TRUE);

	// Wont wait for key input
	nodelay(output.outputStream, TRUE);
}



// ------------------------------------------------------------------------------
//   Refresh all output windows
// ------------------------------------------------------------------------------

void OutputStream::refresh(void) {
	wrefresh(outputStream);
	wrefresh(insectData);
	wrefresh(uavData);
	wrefresh(cameraData);
	wrefresh(flagData);
}



// ------------------------------------------------------------------------------
//   Initialise a global instance of output stream
// ------------------------------------------------------------------------------

OutputStream output;






// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
//   Refresh Data Window
// ------------------------------------------------------------------------------


void printDataWindow(map<const char*, int> in_map, int y, int x)
{
	// Note that all data windows are right aligned
	// Initialise the width.
	int width = 0;

	// Finds the appropriate width for the data window
	map<const char*,int>::iterator it;	
	for (it=in_map.begin(); it!=in_map.end(); ++it)
	{	
		int wordLength = strlen(it->first);
		if (wordLength > width) width = wordLength;
	}


	// Creates a window according to the data width
	WINDOW * window = newwin(in_map.size()+1, width, y, x);


	
	for (it=in_map.begin(); it!=in_map.end(); ++it)
	{
		// If it is in the zero position, it is a title and will be underlined
		if (it->second == 0) { wattron(window, A_UNDERLINE); }  
	  
		// Move to the appropriate position and print the word
	 	mvwprintw(window, it->second, width - strlen(it->first), it->first);

		if (it->second == 0) { wattroff(window, A_UNDERLINE); }
	}

    wrefresh(window);
}



// ------------------------------------------------------------------------------
//   Draw Main Window
// ------------------------------------------------------------------------------
 
 void drawMainWindow(void)
 {
	// Start from scratch
 	erase();

	// If the screen is too small the formatting gets messed up
 	int h, w;
	getmaxyx(stdscr, h, w);
	while (h < 42 || w < 89)
	{
		mvprintw(0,0,"Error: Screen is too small.\n");
		printw("Height = %d < 42\n", h);
		printw("Width = %d < 89", w);
		refresh();
		erase();
		getmaxyx(stdscr, h, w);
	}
	
	// Ensures the formatting is always consistent
	resizeterm(42, 89);
	refresh();
}


// ------------------------------------------------------------------------------
//   Print Main Screen
// ------------------------------------------------------------------------------

void printMainScreen(void) 
{
	// Draws the entire window which the screen is printed in
	drawMainWindow();

	// Set the screen size to the dimensions of the window
	int h, w;
	getmaxyx(stdscr, h, w);

	// Screen title
	attron(A_STANDOUT);
	mvprintw(0, 0, " Insect Tracking Program");
	chgat(-1, A_STANDOUT, 0, NULL);
	
	// Bar for command instructions
	mvchgat(h-2, 0, -1, A_STANDOUT, 0, NULL);
	mvchgat(h-1, 0, -1, A_STANDOUT, 0, NULL);
	
	// Command instructions
	mvprintw(h-2, 2, "q: Quit");	
	mvprintw(h-2, 15, "c: Connect Camera");
	mvprintw(h-2, 35, "r: Refresh screen");
	mvprintw(h-2, 60, "d: View ROI");
	mvprintw(h-1, 2, "p: Pause");
	mvprintw(h-1, 15, "u: UAV Control");
	mvprintw(h-1, 35, "e: Check response");
	mvprintw(h-1, 60, "f: View full frame");
    
	// Title for output stream
	mvprintw(5, 49, "Output Stream");
	
	// Window for output stream
	WINDOW * outputBox = newwin(OUTPUT_STREAM_HEIGHT+2, OUTPUT_STREAM_WIDTH+2, OUTPUT_STREAM_Y-1, OUTPUT_STREAM_X-1);
	box(outputBox, 0, 0);
	
	refresh();
	wrefresh(outputBox);

}


