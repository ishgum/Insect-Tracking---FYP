
#ifndef Input
#define Input

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>


using namespace std;


class myIn {
	char c;
	bool newChar;
	pthread_t thread;
public:
	myIn(void);
	void start(void);
	char getChar();		//Performs destructive read
	void setChar(char inC);	
	bool gotChar();

};

extern myIn input;

static void* readInput(void *threadid);



#endif
