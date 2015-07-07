// Uploading:

A 5V 16Mhz arduino nano is currently on the breadboard.

Use the USB2serial adapter with
Arduino options:

Use arduino Pro or Pro mini

Atmega328 16Mhz 5V

AVRISP mkII
// end uploading

// Memory
2kB SRAM on pro mini
according to https://learn.sparkfun.com/tutorials/data-types-in-arduino
ints use 2 bytes
floats use 4 bytes

yagiTest uses int from ADC reading but RunningAverage lib stores an array of floats.
this buffer is easily the largest structure.
max size of MAF is therefore less than 2048/(2channels*4bytes) = 256

Solutions:
Use something w/ more SRAM
Change RA lib to use ints
// end Memory

// Pulse detection

// end pulse detection