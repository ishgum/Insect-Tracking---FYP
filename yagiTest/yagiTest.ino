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
*/

#include "RunningAverage.h"

#define LEFT       A0
#define RIGHT      A3
#define LEFTLED    9
#define RIGHTLED   7
#define MIDDLELED  8
#define ARDUINO_PWR_V  5//4.55 // about 4.55V on USB //5.0V ok with lipo
#define MAFSIZE    200// 256 absolute max, 200 probably safe
#define THRESHOLD  0.1 // V, for max difference between Left and Right considered "the same"

//Display Modes
#define PRINT_EVERY_N  800//1//800
#define DIR_MAG      //display strongest dir & mag
#define RAW         //display raw values on RHS
#define CRAPH          //display mag with .'s
//#define HIGHLIGHT_PEAK //for LEFT A0 only , only active when RAW is defined


void setup() {
  Serial.begin(115200);    //for speed!
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFTLED, OUTPUT);
  pinMode(RIGHTLED, OUTPUT);
  pinMode(MIDDLELED, OUTPUT);
  digitalWrite(LEFTLED, LOW);
  digitalWrite(RIGHTLED, LOW);
  digitalWrite(MIDDLELED, LOW);
  Serial.println("STRONGEST:\tMAGNITUDE:");
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

void loop() {
  //Sample
  left_b.addValue(analogRead(LEFT));
  right_b.addValue(analogRead(RIGHT));
  
  //Compare
  average_left = left_b.getAverage()*ARDUINO_PWR_V/1023;
  average_right = right_b.getAverage()*ARDUINO_PWR_V/1023;
  diff =  average_left-average_right;
  mag = abs(diff);
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

  N++;
  if (N >PRINT_EVERY_N){
      N =0;
      Serial.println(output);
  }
  //delay(00);
  delayMicroseconds(100); //max ADC speed given as 100us
}
