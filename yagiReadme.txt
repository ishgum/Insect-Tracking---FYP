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

//Radio
Easiest way to accomodate radio is to add a delay in serial write function.
Modify Print.c.pp
Print.cpp can be found in arduino install folder; Arduino\hardware\arduino\avr\cores\arduino\Print.cpp
In Print.cpp add in commented region to write function:

/* default implementation: may be overridden */

size_t Print::write(const uint8_t *buffer, size_t size)
{

  size_t n = 0;

  while (size--) {

/************************************************************************************************************************************/

	 delay(2);	// Added by Dylan Mackie to make this work with HAK_96 radio.
/***********************************************************************************************************************************/

    n += write(*buffer++);

  }

  return n;

}

//end radio