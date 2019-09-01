
#ifndef HardwareDefinitions_h
#define HardwareDefinitions_h

#include "Arduino.h"
#include "GlobalVariables.h"

// ANIMATION CONTROL VARIABLES
int minBlinkSpeed = 3000; // max time in ms. that an LED will take to fade from on to off and back to on. after this time is reached,
// the time held at on will begin to increase. once the time held on reaches maximum, the LED will convert to always on (ie, not blinking)
int maxBlinkSpeed = 250; // min time in ms. that an LED will take to fade from on to off and back to on. when blinkDuration is
// at 0, this is the maximum speed at which the LED will blink.
int maxBlinkHold = 1000; // max time in ms. that an LED will stay on between blinks before just converting to always on
int maxBlinkDuration = 60; // max time in seconds that an LED will blink for after the corresponding button is pressed.


// HARDCODED PANEL DEFINITIONS:
//const int numled = 20; 
//#define numled 20
// First, define all the regions by absolute button number. There MUST NOT be repeats, or the code will not compile
// (it wouldn't make much sense to have multiple regions recorded with a single button, if that is the case rename the region
// even if only for the software.
#define NOREGION -1
#define REGION1 47
#define REGION2 42
#define REGION3 32
#define REGION4 33

// animation presets
#define T1SCALE 30
#define T1OFFSET -5
#define T2SCALE 30
#define T2OFFSET 10
#define T3SCALE 30
#define T3OFFSET 20
#define T4SCALE 30
#define T4OFFSET 30

// next, define all the LEDs:
// each LED needs to know: which region it belongs to, how it scales brightness with num button presses (scale rate and offset if it doesn't turn on right away or is always on from the beginning)
// these last two can probably be sorted into tiers, ie. T1 T2 and T3 with fixed numbers for each tier
// calculated from this: max brightness/ hue (defined by a 3 byte color)- I say MAXIMUM because the LED may be blinking, and needs to know what the target level is
// blink coefficients for speed and hold, calculated from the last time the corresponding button was pressed and the max blink duration
// timestamp of last visual update, (or timestamp of when the last blink started) - if we want the blinks to be time-accurate and independant of update rate. Otherwise assume 30 hz
// and just increment them based on that.
// target hue, if different from the standard animation.


// this is sort of a hacked way of implementing a struct- we are just densly packing data into an array.
// it works . . . as long as you don't get off by one anywhere. a better way to do this would be to create
// an LED object with get and set methods, but that takes more effort and possibly more memory.
// in this array, the first dimension is the LED number, the second dimension is 3 values: attached button, scale rate, offset.
// scale rate is measured in how many button presses it takes to reach full scale, so a larger number means slower growth.
// offset is measured in button presses before the light starts to respond, so 0 means start with first press, and a negative
// value means it starts on before any buttons are pressed.
// declaring this table here means every entry MUST be populated. If the light is not used in the panel, just set button association
// to NOREGION and leave the other entries as anything.
// ideally, this AND the button definitions would be loaded from a configuration file at some point.
long LEDLookupTable[numled][3]={
  {REGION3,T3SCALE,T3OFFSET}, // LED0
  {REGION3,T2SCALE,T2OFFSET}, // LED1
  {REGION3,T2SCALE,T2OFFSET}, // LED2
  {REGION3,T2SCALE,T2OFFSET}, // LED3
  {REGION3,T1SCALE,T1OFFSET}, // LED4
  {REGION3,T2SCALE,T2OFFSET}, // LED5
  {REGION1,T2SCALE,T2OFFSET}, // LED6
  {REGION1,T2SCALE,T2OFFSET}, // LED7
  {REGION1,T1SCALE,T1OFFSET}, // LED8
  {REGION1,T2SCALE,T2OFFSET}, // LED9
  {REGION2,T1SCALE,T1OFFSET}, // LED10
  {REGION2,T2SCALE,T2OFFSET}, // LED11
  {REGION2,T3SCALE,T3OFFSET}, // LED12
  {REGION2,T4SCALE,T4OFFSET}, // LED13
  {NOREGION,0,0}, // LED14
  {REGION4,T3SCALE,T3OFFSET}, // LED15
  {REGION4,T2SCALE,T2OFFSET}, // LED16
  {REGION4,T2SCALE,T2OFFSET}, // LED17
  {REGION4,T1SCALE,T1OFFSET}, // LED18
  {REGION4,T2SCALE,T2OFFSET}}; // LED19


// This function is hardcoded and must be updated to match the panel definition.
// It prints all of the current values by region, for all existing regions.
void printButtonPressesByRegion(){
  Serial.println("All recorded data:");
  for (int i = 0; i < numButtons; i++){
    booReadyToPrint = true;
    switch(i){
      case REGION1:
        Serial.print("REGION1, "); break;
      case REGION2:
        Serial.print("REGION2, "); break;
      case REGION3:
        Serial.print("REGION3, "); break;
      case REGION4:
        Serial.print("REGION4, "); break;
      default:
        booReadyToPrint = false;
        break;
    }
    if (booReadyToPrint == true){Serial.print(lngButtonCounter[i]);Serial.print(",");Serial.println(lngButtonDisplayCounter[i]);}
  }
}

#endif
