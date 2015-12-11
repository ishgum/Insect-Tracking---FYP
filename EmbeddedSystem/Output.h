
#ifndef Output
#define Output

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include <ncurses.h>
//#include "OutputStream.h"

#define MAX_NUM_ROWS 20

using namespace std;

/*

class myIn {
	int rows, width, x_pos, y_pos;
	map<const char*, int > entryMap;
	char* title;
	WINDOW * window;
public:
	myIn(void);

	void start(void);
	char getChar();		//Performs destructive read
	void setChar(char inC);	
	bool gotChar();

};

extern myIn input;

static void* readInput(void *threadid);
*/



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





#endif
