/****************************************************************************
 *
 *
 * @file ui_input.h
 *
 * @brief Provides functions for inputs into the user interface
 *
 * Main function just reads the input and if the character matches one of the
 * commands it will change a flag so that the command gets executed
 * 
 *
 * @author Michael McAdam
 *
 *
 ****************************************************************************/


#ifndef UI_INPUT_H_
#define UI_INPUT_H_


// ------------------------------------------------------------------------------
//   Includes
// ------------------------------------------------------------------------------

#include <ncurses.h>

#include "ui_output.h"


// ------------------------------------------------------------------------------
//   Namespaces
// ------------------------------------------------------------------------------

using namespace std;



// ------------------------------------------------------------------------------
//   Global flags
// ------------------------------------------------------------------------------


extern bool contPause, contCam, contQuit, contUAV, contDebugROI, contDebugFull, contRefresh, contTest;



// ----------------------------------------------------------------------------------
//   Functions
// ----------------------------------------------------------------------------------


// Reads the input from the ncurses window and changes the appropriate flags
void inputControl (void);

// Prints the flag parameters
void printFlagParameters (int y, int x);

// Refreshes the flag data
void printFlagOutput (void);





#endif //UI_INPUT_H_
