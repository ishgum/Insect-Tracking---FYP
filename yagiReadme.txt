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
9600 bits per second
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

Also need to add to println function if you want to use it:

size_t Print::println(void)
{

  size_t n = print('\r');

/************************************************************************************************************************************/

  delay(2);		// Added by Dylan Mackie to make this work with HAK_96 radio.

  n += print('\n');
  delay(2);	// Added by Dylan Mackie to make this work with HAK_96 radio.
/***********************************************************************************************************************************/
  return n;

}

Note that these changes mean that radio is sloooow:
E.G. To send 20 Bytes over a serial connection,
* 11520: 13.9ms
	actually takes 1 - 2 ms on direct 11520 serial
	>> CHECK continuous rate & num of characters & character size for this case
* 9600: 56.7ms

20*(2+(1000*1/(9600/8)))=56.6667 ms
20*(0+(1000*1/(115200/8)))=1.3889 ms

Now, 57ms is clearly a large enough period to miss pulses.
Either use interrupts on pulse signal, or break printing into smaller tasks.

Testing with all display #defines takes about 10ms per displayData call
Testing with just time and dir display #defines takes about 3ms per displayData call
	
//end radio