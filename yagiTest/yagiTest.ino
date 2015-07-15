/* ENEL400 Insect Tracking FYP
  Dylan Mackie, Michael Jones,
  2015

Just reading & comparing two ADC's
has provision to implement a Moving average filter,
just change MAFSIZE #define to size of MA buffer.
Toggles 3 LED's to indicate which ADC signal is strongest.
Has various display modes to be (un)commented as needed.
These should be commented out for headless operation for max speed,
though at 115200baud this program should still be able to get a
sample at least every 5ms.

Max ADC read rate is ~0.1ms
Can get ~ 1 sample per ms w/ DIR_MAG.
digitalWrite typ takes <20us so can do this every loop

Has basic detect & compare pulses functionality
When no pulses are being detected the state can be queried by sending one character
to the serial port. Send a msg by selecting CR or NL in serial monitor window,
and hit enter in the msg window.

TODO:
> investigate new error when using MAFSIZE = 1
> create pulse test code
> check time display
> fix  pulse averaging
>possibly trim rising & falling edges of pulse detect
      to increase accuracy (if >3 samples achieved)
>Have some overall gain information
  >scale comparison based on this gain information
>remove known pulse values from averaging buffer for noise floor
  calculation
*/

#include "display.h"
#include "distance.h"
#include "RunningAverage.h"

// Settings
#define MODE                   2 // 0 = pulse, 1 = continuous (with MAF), 2 = super simple continuous
#define SIMPLE_PULSE   //defined: uses basic check against MAF, then delays 5ms and samples to determine pulse value,
                          //otherwise use a more complicated mode that averages all samples during the pulse.
#define TEST_MODE                      //Use test array not ADC readings
#define ARDUINO_PWR_V          5      //4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAFSIZE                50    // 256 absolute max, 200 probably safe
#define DIFFERENCE_THRESHOLD   0.1     // V, for max difference between Left and Right considered "the same" (0 to 5 valid)
#define PULSE_THRESHOLD        0.5     // V, the amount the RSSI amplitude has to be greater than the averaged
                                        // amplitude to detect a pulse (0 to 5 valid)



// Pin dfns
#define LEFT_PIN       A0
#define RIGHT_PIN      A3
#define LEFTLED        9
#define RIGHTLED       7
#define MIDDLELED      8

enum pulse_status_t {NO, YES, FALLING_EDGE};
float test_array_l[MAFSIZE];
float test_array_r[MAFSIZE];

void setup() {
  Serial.begin(115200);    //for speed!
  init_LEDs();
  Serial.println("STRONGEST:\tMAGNITUDE:");

  // fill test array
  init_test_arrays();

  // Select mode based on MODE #define
  #if MODE == 0
    pulse();
  #elif MODE == 1
    continuous();
  #else
    simple();
  #endif
}

RunningAverage left_b(MAFSIZE);
RunningAverage right_b(MAFSIZE);
float average_left = 0;
float average_right = 0;
float diff = 0;
float mag = 0;
String dir = "uninitialised";
int N = 0;


void init_test_arrays(void) {
  for (int i = 0; i < MAFSIZE - 5; i++) {
    test_array_l[i] = 0.0;
  }
  test_array_l[MAFSIZE - 5] = 0.3;
  test_array_l[MAFSIZE - 4] = 1.0;
  test_array_l[MAFSIZE - 3] = 1.0;
  test_array_l[MAFSIZE - 2] = 1.0;
  test_array_l[MAFSIZE - 1] = 0.3;
}

// simple method to return next test_array sample
float get_test_sample(float * _sample_array, int _size){
  // Init index
  static int _idx = 0;
  //increment index
  _idx++;
  // check for index exceeding bounds
  if (_idx == _size){
    _idx = 0;
  }
  return _sample_array[_idx];
}

float get_random_sample(void){
  // return value between 0.5V an 2.5V
  return random(0,20)/10.0+0.5;
}




// Super simple mode, prints as fast as possible, typ 1 - 2ms
void simple(void) {
  while(1){
    //Serial.print(millis()-start_time);
    //Serial.print("\t");
    Serial.print((analogRead(LEFT_PIN))*ARDUINO_PWR_V/1023.0);
    Serial.print("\t");
    Serial.println((analogRead(RIGHT_PIN))*ARDUINO_PWR_V/1023.0);
  }
}

/*  Continuous Mode loop:
    Sample two channels as fast as possible (+ 100us delay),
    Add each sample to one of two RollingAverage buffers,
    and find the average.
    If the average for one buffer is greater than the other (within some tolerance DIFFERENCE_THRESHOLD),
    Then that channel is said to have a greater RSSI, which is indicated by the LED's,
    and Serial output if there are suitable display defines.
    LED's updated every sample.
    Serial updated every PRINT_EVERY_N sample to slow down display.
    */
void continuous(void) {
  float current_left = 0;
  float current_right = 0;
  while (1) {
    //Sample
    current_left = analogRead(LEFT_PIN)*ARDUINO_PWR_V/1023.0;
    current_right = analogRead(RIGHT_PIN)*ARDUINO_PWR_V/1023.0;
    left_b.addValue(current_left);
    right_b.addValue(current_right);

    //Compare
    average_left = left_b.getAverage();
    average_right = right_b.getAverage();
    display_data(average_left, average_right);
    delayMicroseconds(100); //max ADC speed given as 100us
    
    // Check for incoming serial messages, and print status if we get anything
    // Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
    if (Serial.available() > 0) {
      serial_response(current_left, current_right, average_left, average_right);
    }
  }
}



/*  Pulse Mode Loop:
    Sample two channels as fast as possible (+ 100us delay),
    Add each sample to one of two RollingAverage buffers,
    and find the average.
    The noise floor is defined as these averages.
    If the average for one buffer is greater than noise floor + PULSE_THRESHOLD,
    Then a pulse is said to be occuring.
    pulse_state is set to YES (pulse occuring).
    when pulse no longer occurs pulse_state becomes FALLING,
    the average amplitude of the pulse is then determined for both channels,
    then pulse_state = NO.

    The average amplitude for each channel during the pulse is then compared (with DIFFERENCE_THRESHOLD again),
    if one is greater then,
    that channel is said to have a greater RSSI, which is indicated by the LED's,
    and Serial output if there are suitable display defines.
    LED's updated every pulse.
    Serial updated every pulse.
    */
void pulse(void) {
  int test_indx = 0;  // for debugging using a fixed test array
  float current_left = 0;
  float current_right = 0;
  int pulse_start = 0;
  int pulse_end = 0;
  int pulse_sample_num = 0;  //number of samples of pulse
  float pulse_left_av, pulse_right_av, pulse_left_sum, pulse_right_sum = 0;
  bool left_over_thresh, right_over_thresh = false;
  pulse_status_t pulse_status = NO;
  Serial.println("filling buffer\n");
  //wait until bufffer is full
  while (left_b.getCount() < MAFSIZE) {
    left_b.addValue(analogRead(LEFT_PIN)*ARDUINO_PWR_V / 1023.0);
    right_b.addValue(analogRead(RIGHT_PIN)*ARDUINO_PWR_V / 1023.0);
  }
  //int pulse_sample_num = 0;
  Serial.println("Buffer full\n");

  start_time = millis();

  while (1) {
    //Sample
    current_left = analogRead(LEFT_PIN) * ARDUINO_PWR_V / 1023.0;
    current_right = analogRead(RIGHT_PIN) * ARDUINO_PWR_V / 1023.0;

    // use test array
    //    current_left = test_array_l[test_indx];
    //    current_right = test_array_r[test_indx++];
    //    if (test_indx >MAFSIZE){
    //      test_indx = 0;
    //    }
    // end debug test

    left_b.addValue(current_left);
    right_b.addValue(current_right);

    average_left = left_b.getAverage();
    average_right = right_b.getAverage();


    // Find if reading exceeds noise floor
    left_over_thresh = current_left >= average_left + PULSE_THRESHOLD;
    right_over_thresh = current_right >= average_right + PULSE_THRESHOLD;

#ifdef SIMPLE_PULSE
    if ((left_over_thresh || right_over_thresh)) {
      if (pulse_sample_num < 4) { //if one of first 4 pulses on rising edge; ignore
        pulse_sample_num++;
      } else {
        display_data(current_left, current_right);
        delay(20);
        digitalWrite(LEFTLED, LOW);
        digitalWrite(RIGHTLED, LOW);
        digitalWrite(MIDDLELED, LOW);
        pulse_sample_num = 0;
        if (Serial.available() > 0) Serial.println("buzz");
      }
    }
#else
    switch (pulse_status) {

      case NO:  // see if pulse starts on current sample
        //Serial.println(average_right);
        //Serial.print("\t");

        if (left_over_thresh || right_over_thresh) { //if pulse on either channel detected
          pulse_start = left_b.getIndex();
          pulse_end = left_b.getIndex();
          pulse_status = YES;
        }
        break;
      case YES:  // pulse was detected on last sample
        //Serial.println("YES\n");
        if (left_over_thresh || right_over_thresh) { //if pulse on either channel detected
          pulse_end = left_b.getIndex();
          pulse_status = YES;
        } else {
          pulse_status = FALLING_EDGE;
        }
        break;

      case FALLING_EDGE:   // Pulse ended, determine averages for pulse window
        Serial.println("FALLING\n");
        pulse_sample_num = 0;
        if (pulse_end < pulse_start) { // pulse wrapped around in buffer
          for (int i = pulse_start; i <= MAFSIZE; i++) {
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += right_b.getElement(i);
            pulse_sample_num++;
          }
          for (int i = 0; i <= pulse_end; i++) {
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += right_b.getElement(i);
            pulse_sample_num++;
          }
        } else {
          for (int i = pulse_start; i <= pulse_end; i++) {
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += right_b.getElement(i);
            pulse_sample_num++;
          }
        }
        Serial.print(pulse_start);
        Serial.print("\t");
        Serial.print(pulse_end);
        Serial.print("\t");
        Serial.print(pulse_left_sum);
        Serial.print("\t");
        Serial.println(pulse_right_sum);

        pulse_left_av = pulse_left_sum / (pulse_sample_num);
        pulse_right_av = pulse_right_sum / (pulse_sample_num);
        display_data(pulse_left_av, pulse_right_av);
        pulse_status = NO;
        break;
    }
#endif
    //delay(00);
    delayMicroseconds(100); //max ADC speed given as 100us

    // Check for incoming serial messages, and print status if we get anything
    // Send a msg by selecting CR or NL in serial monitor window, and sending a blank msg.
    if (Serial.available() > 0) {
      serial_response(current_left, current_right, average_left, average_right);
    }

  }
  // Serial.println(right_over_thresh);
  //Serial.print("\t");
  //Serial.println(current_right-average_right-PULSE_THRESHOLD);

}




void loop() {}
