/* Just reading & comparing two ADC's
has provision to implement a Moving average filter,
just change MAFSIZE #define to size of MA buffer.
Should toggle 3 LED's to indicate which ADC signal is strongest.
Has various display modes to be (un)commented as needed.
These should be commented out for headless operation for max speed,
though at 115200baud this program should still be able to get a
sample at least every 5ms.
Max ADC read rate is ~0.1ms
Can get ~ 1 sample per ms w/ DIR_MAG

Added UNTESTED functionality to:
detect & compare pulses

TODO:
possibly trim rising & falling edges to increase accuracy (if >3 samples achieved)
*/

#include "RunningAverage.h"

// Settings
#define PULSE_MODE //otherwise continuous
#define ARDUINO_PWR_V  5//4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAFSIZE    200// 256 absolute max, 200 probably safe
#define THRESHOLD  0.1 // V, for max difference between Left and Right considered "the same"

//Display Modes
#define PRINT_EVERY_N  800//1//800
#define DIR_MAG      //display strongest dir & mag
#define RAW         //display raw values on RHS
#define CRAPH          //display mag with .'s
//#define HIGHLIGHT_PEAK //for LEFT A0 only , only active when RAW is defined

// Pin dfns
#define LEFT_PIN       A0
#define RIGHT_PIN      A3
#define LEFTLED    9
#define RIGHTLED   7
#define MIDDLELED  8

enum pulse_status_t {NO, YES};

void setup() {
  Serial.begin(115200);    //for speed!
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(LEFTLED, OUTPUT);
  pinMode(RIGHTLED, OUTPUT);
  pinMode(MIDDLELED, OUTPUT);
  digitalWrite(LEFTLED, LOW);
  digitalWrite(RIGHTLED, LOW);
  digitalWrite(MIDDLELED, LOW);
  Serial.println("STRONGEST:\tMAGNITUDE:");
  #ifdef PULSE_MODE
    pulse();
  #else
    continuous();
  #endif
}

RunningAverage left_b(MAFSIZE);
RunningAverage right_b(MAFSIZE);
float average_left = 0;
float average_right = 0;
String output = "";
float diff =0;
float mag = 0;
String dir = "uninitialised";
int N = 0;

void continuous(void){
  while(1){
    //Sample
    left_b.addValue(analogRead(LEFT_PIN));
    right_b.addValue(analogRead(RIGHT_PIN));
    
    //Compare
    average_left = left_b.getAverage()*ARDUINO_PWR_V/1023;
    average_right = right_b.getAverage()*ARDUINO_PWR_V/1023;
    display_data(average_left, average_right);
    delayMicroseconds(100); //max ADC speed given as 100us
  }
}

void pulse(void){
  int current_left = 0;
  int current_right = 0;
  int pulse_start = 0;
  int pulse_end = 0;
  int pulse_sample_num = 0;  //number of samples of pulse
  pulse_status_t pulse_status = NO;

  //wait until bufffer is full
  while(left_b.getCount() < MAFSIZE){
    left_b.addValue(analogRead(LEFT_PIN));
    right_b.addValue(analogRead(RIGHT_PIN));
  }
  
  while(1){
    //Sample
    current_left = analogRead(LEFT_PIN);
    current_right = (analogRead(RIGHT_PIN);
    left_b.addValue(current_left);
    right_b.addValue(current_right);
    
    // Compare raw adc
    average_left = left_b.getAverage();
    average_right = right_b.getAverage();
    
    switch (pulse_status){
       // Find if reading exceeds noise floor     
      left_over_thresh = current_left >= average_left + PULSE_THRESHOLD;
      right_over_thresh = current_left >= average_left + PULSE_THRESHOLD;
      
      case NO:  // see if pulse starts on current sample
        if (left_over_thresh || right_over_thresh){  //if pulse on either channel detected
          pulse_start = left_b.getIndex();
          pulse_end = left_b.getIndex();
          pulse_status = YES
        }
        break;
      case YES:  // pulse was detected on last sample 
        if (left_over_thresh || right_over_thresh){  //if pulse on either channel detected
          pulse_end = left_b.getIndex();
          pulse_status = YES;
        }else{
          pulse_status = FALLING_EDGE;
        }
        break;
  
      if (pulse_status == FALLING_EDGE){    // Pulse ended, determine averages for pulse window
        pulse_sample_num = 0;
        if (pulse_end < pulse_start){ // pulse wrapped around in buffer
          for (int i = pulse_start; i<= MAFSIZE; i++;){
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += left_b.getElement(i);
            pulse_sample_num++;
          }
          for (int i = 0; i<= pulse_end; i++;){
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += left_b.getElement(i);
            pulse_sample_num++;
          }
        }else{
          for (int i = pulse_start; i<= pulse_end; i++;){
            pulse_left_sum += left_b.getElement(i);
            pulse_right_sum += left_b.getElement(i);
            pulse_sample_num++;
          }
        }
        pulse_left_av = pulse_left_sum*ARDUINO_PWR_V/(pulse_sample_num*1023);
        pulse_right_av = pulse_right_sum*ARDUINO_PWR_V/(pulse_sample_num*1023;
        display_data(pulse_left_av, pulse_right_av);
      }
      pulse_status = NO;
      break;
    }
    //delay(00);
    delayMicroseconds(100); //max ADC speed given as 100us
  }
}

void display_data(float average_left, float average_right){
  float diff =  average_left-average_right;
  float = abs(diff);
  if (diff>THRESHOLD){
    dir = "Left";
    digitalWrite(LEFTLED, HIGH);
    digitalWrite(RIGHTLED, LOW);
    digitalWrite(MIDDLELED, LOW);
  }else if (diff < -THRESHOLD){
    dir = "Right";
    digitalWrite(LEFTLED, LOW);
    digitalWrite(RIGHTLED, HIGH);
    digitalWrite(MIDDLELED, LOW);
  }else{
    dir = "Middle";
    digitalWrite(LEFTLED, LOW);
    digitalWrite(RIGHTLED, LOW);
    digitalWrite(MIDDLELED, HIGH);
  }
  
  //Create Serial output
  // Note that large serial msg's can a couple of ms at 115200
  // and 10's of ms at 9600 baud
  output = "";
  #ifdef DIR_MAG
   // output = "Strongest:\t";
    output += dir+"\t";
    output += mag;
  #endif
  #ifdef CRAPH
    output += "\t";
    for (int i = 0; i<mag*10&&i<100; i++){
      output += ".";
      if(i==99){
        output += "+!";
      }
    }
  #endif
  #ifdef RAW
    output += "\tRAW: L:\t";
    output += String(average_left);
    #ifdef HIGHLIGHT_PEAK
      if (average_left>1){
        Serial.println("\n****\n");
      }
    #endif
    output += "\tR:\t";
    output += String(average_right);
  #endif
  
  if defined(PULSE_MODE){
    Serial.println(output);
  }else{
    N++;
    if (N >PRINT_EVERY_N){
        N =0;
        Serial.println(output);
    }
  }
}
