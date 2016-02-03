/****************************************************************************
 *
 *
 * @file ui_output.h
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


#ifndef UI_OUTPUT_H_
#define UI_OUTPUT_H_


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include <map>
#include <string.h>

#include <ncurses.h>
#include "parameters.h"


// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace std;


// ------------------------------------------------------------------------------
//   Defines
// ------------------------------------------------------------------------------

#define MAX_NUM_ROWS 20



// ----------------------------------------------------------------------------------
//   OutputStream Class
// ----------------------------------------------------------------------------------
/*
 * OutputStream Class
 *
 * This manages all of the ui output windows including the output stream and the data
 * windows for the insect, the uav, the camera and the flags.
 * 
 */


class OutputStream {

private:

	
public:

	OutputStream(void);

	// Initialises all of the windows
	void init(void);
	
	// Refreshes all of the windows
	void refresh(void);

	WINDOW * outputStream;
	WINDOW * insectData;
	WINDOW * uavData;
	WINDOW * cameraData;
	WINDOW * flagData;

};




// ------------------------------------------------------------------------------
//   Functions
// ------------------------------------------------------------------------------

// Prints the main screen
void printMainScreen(void);

// Function for printing a generic data window
void printDataWindow(map<const char*, int> in_map, int, int);



// ------------------------------------------------------------------------------
//   Globally defined variables
// ------------------------------------------------------------------------------

// This should be reworked so that this class is a singleton
extern OutputStream output;



#endif //UI_OUTPUT_H
