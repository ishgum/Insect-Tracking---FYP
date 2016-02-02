
#ifndef UI_INPUT_H_
#define UI_INPUT_H_


#include <ncurses.h>

#include "ui_output.h"

using namespace std;


void inputControl (void);

void printFlagParameters (int y, int x);

void printFlagOutput (void);



extern bool contPause, contCam, contQuit, contUAV, contDebugROI, contDebugFull, contRefresh;



#endif //INPUT_H_
