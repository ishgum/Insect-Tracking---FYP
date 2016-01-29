
#ifndef Output
#define Output

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include <string.h>

#include <ncurses.h>
#include "parameters.h"

#define MAX_NUM_ROWS 20

using namespace std;




class OutputStream {
	
public:
	OutputStream(void);

	void init(void);
	void print(const char* s);
	void refresh(void);
	WINDOW * outputStream;
	WINDOW * insectData;
	WINDOW * uavData;
	WINDOW * cameraData;
	WINDOW * flagData;

};


extern OutputStream output;
extern char outputBuffer[50];

void printMainScreen(void);

void printDataWindow(map<const char*, int> in_map, int, int);





#endif
