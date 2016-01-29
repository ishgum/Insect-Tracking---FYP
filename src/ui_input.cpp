
#include "ui_input.h"


using namespace std;

myIn::myIn() {
	c = 0;
	newChar = false;
}




void myIn::start() {
	pthread_create(&thread, NULL, readInput, NULL);
}

void myIn::setChar (char inC) {
	newChar = true;
	c = inC;
}


char myIn::getChar (void) {
	char tempC = c;
	c = 0;
	newChar = false;
	return tempC;
}


bool myIn::gotChar (void) {
	return newChar;
}



myIn input;

void* readInput(void *args)
{
	char c;
	while (1) {
		scanf("%c", &c);
		if (c != '\n') input.setChar(c);
	}
}
