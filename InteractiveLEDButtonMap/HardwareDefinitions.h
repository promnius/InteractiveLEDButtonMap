
#ifndef HardwareDefinitions_h
#define HardwareDefinitions_h

#include "Arduino.h"
#include "GlobalVariables.h"

// ANIMATION CONTROL VARIABLES
int minBlinkSpeed = 4000; // max time in ms. that an LED will take to fade from on to off and back to on. after this time is reached,
// the time held at on will begin to increase. once the time held on reaches maximum, the LED will convert to always on (ie, not blinking)
int maxBlinkSpeed = 200; // min time in ms. that an LED will take to fade from on to off and back to on. when blinkDuration is
// at 0, this is the maximum speed at which the LED will blink.
int maxBlinkHold = 1000; // max time in ms. that an LED will stay on between blinks before just converting to always on
int maxBlinkDuration = 2400; // max time in seconds that an LED will blink for after the corresponding button is pressed.

// animation presets
#define T1S 200
#define T2S 200
#define T3S 300
#define T4S 300
#define T5S 300
#define T6S 400
#define T7S 400
#define T8S 400

#define T1M -20
#define T2M 40
#define T3M 80
#define T4M 120
#define T5M 160
#define T6M 200
#define T7M 240
#define T8M 280


// HARDCODED PANEL DEFINITIONS:
//const int numled = 20; 
//#define numled 20
// First, define all the regions by absolute button number. There MUST NOT be repeats, or the code will not compile
// (it wouldn't make much sense to have multiple regions recorded with a single button, if that is the case rename the region
// even if only for the software.
#define NOREGION -1
#define ROGERSPARK 79
#define WESTRIDGE 78
#define NORTHPARK 69
#define FORESTGLEN 68
#define JEFFERSONPARK 67
#define NORWOODPARK 66
#define EDISONPARK 65
#define OHARE 64
#define DUNNING 63
#define PORTAGEPARK 61
#define IRVINGPARK 74
#define ALBANYPARK 70
#define LINCOLNSQUARE 75
#define EDGEWATER 77
#define UPTOWN 76
#define LAKEVIEW 33
#define NORTHCENTER 32
#define AVONDALE 59
#define HERMOSA 58
#define BELMONTCRAGIN 60
#define MONTCLARE 62
#define AUSTIN 56
#define HUMBOLDTPARK 55
#define LOGANSQUARE 57
#define WESTTOWN 36
#define LINCOLNPARK 34
#define NEARNORTHSIDE 35
#define LOOP 38
#define NEARWESTSIDE 37
#define EASTGARFIELDPARK 53
#define WESTGARFIELDPARK 54
#define NORTHLAWNDALE 52
#define SOUTHLAWNDALE 51
#define LOWERWESTSIDE 39
#define SOUTHLOOP 40
#define DOUGLAS 43
#define ARMOURSQUARE 41
#define BRIDGEPORT 42
#define MCKINLEYPARK 50
#define BRIGHTONPARK 49
#define ARCHERHEIGHTS 5
#define GARFIELDRIDGE 0
#define CLEARING 1
#define WESTELSDON 3
#define GAGEPARK 4
#define NEWCITY 48
#define FULLERPARK 45
#define BRONZEVILLE 46
#define OAKLAND 44
#define KENWOOD 47
#define HYDEPARK 16
#define WASHINGTONPARK 18
#define ENGLEWOOD 20
#define WESTENGLEWOOD 13
#define CHICAGOLAWN 14
#define WESTLAWN 2
#define ASHBURN 15
#define AUBURNGRESHAM 12
#define GREATERGRANDCROSSING 21
#define WOODLAWN 17
#define SOUTHSHORE 19
#define SOUTHCHICAGO 22
#define CALUMETHEIGHTS 25
#define AVALONPARK 23
#define BURNSIDE 26
#define CHATHAM 24
#define BEVERLY 10
#define WASHINGTONHEIGHTS 11
#define ROSELAND 7
#define PULLMAN 27
#define SOUTHDEERING 29
#define EASTSIDE 28
#define HEGEWISCH 31
#define RIVERDALE 30
#define WESTPULLMAN 6
#define MORGANPARK 8
#define MOUNTGREENWOOD 9


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
//long LEDLookupTable[numled][3];

long LEDLookupTable[numled][3]={
  {OHARE, T1S, T1M},
  {NOREGION, 0 ,0},
  {OHARE, T2S,T2M},
  {OHARE, T3S, T3M},
 
  {OHARE, T4S, T4M},
  {OHARE, T5S, T5M},
  {NOREGION, 0 ,0},
  {OHARE, T6S, T6M},
  {NOREGION, 0, 0},
  {OHARE, T7S, T7M},
  {DUNNING, T1S, T1M},
  {NOREGION, 0, 0},
  {OHARE, T8S, T8M},
  {NORWOODPARK, T1S, T1M},
  {NOREGION, 0, 0},
  {NORWOODPARK, T2S, T2M},
  {NOREGION, 0, 0},
  {NOREGION, 0, 0},
  {EDISONPARK, T1S, T1M},
  {NOREGION, 0, 0},
  {NORWOODPARK, T3S, T3M},
  {NOREGION, 0, 0},
  {FORESTGLEN, T1S, T1M},
  {JEFFERSONPARK, T1S, T1M},
  {FORESTGLEN, T2S, T2M},
  {NOREGION, 0, 0},
  {JEFFERSONPARK, T2S, T2M},
  {NORWOODPARK, T4S, T4M},
  {DUNNING, T2S, T2M},
  {PORTAGEPARK, T1S, T1M},

  {NOREGION, 0, 0},
  {PORTAGEPARK, T2S, T2M},
  {ALBANYPARK, T1S, T1M},
  {NORTHPARK, T1S, T1M},
  {IRVINGPARK, T1S, T1M},
  {NOREGION, 0, 0},
  {HERMOSA, T1S, T1M},
  {PORTAGEPARK, T3S, T3M},
  {BELMONTCRAGIN, T1S, T1M},
  {DUNNING, T3S, T3M},
  {MONTCLARE, T1S, T1M},
  {MONTCLARE, T2S, T2M},
  {AUSTIN, T1S, T1M},
  {NOREGION,0,0},
  {AUSTIN, T2S, T2M},
  {BELMONTCRAGIN, T2S, T2M},
  {HERMOSA, T2S, T2M},
  {LOGANSQUARE, T1S, T1M},
  {HUMBOLDTPARK, T1S, T1M},
  {HUMBOLDTPARK, T2S, T2M},
  {WESTGARFIELDPARK, T1S, T1M},
  {NOREGION,0,0},
  {AUSTIN, T3S, T3M},
  {AUSTIN, T4S, T4M},
  {NORTHLAWNDALE, T1S, T1M},
  {WESTGARFIELDPARK, T2S, T2M},
  {NORTHLAWNDALE, T2S, T2M},

  {NOREGION, 0, 0},
  {SOUTHLAWNDALE, T1S, T1M},
  {ARCHERHEIGHTS, T1S, T1M},
  {GARFIELDRIDGE, T1S, T1M},
  {NOREGION, 0, 0},
  {GARFIELDRIDGE, T2S, T2M},
  {GARFIELDRIDGE, T3S, T3M},
  {GARFIELDRIDGE, T4S, T4M},
  {CLEARING, T1S, T1M},
  {CLEARING, T2S, T2M},
  {GARFIELDRIDGE, T5S, T5M},
  {WESTELSDON, T1S, T1M},
  {WESTLAWN, T1S, T1M},
  {NOREGION, 0, 0},
  {WESTLAWN, T2S, T2M},
  {ASHBURN, T1S, T1M},
  {NOREGION, 0, 0},
  {ASHBURN, T2S, T2M},
  {NOREGION, 0, 0},
  {NOREGION, 0, 0},
  {MOUNTGREENWOOD, T1S, T1M},
  {NOREGION, 0, 0},
  {MOUNTGREENWOOD, T2S, T2M},
  {NOREGION, 0, 0},
  {MOUNTGREENWOOD, T3S, T3M},
  {NOREGION, 0, 0},
  {ASHBURN, T3S, T3M},
  {NOREGION, 0, 0},
  {WESTLAWN, T3S, T3M},
  {CHICAGOLAWN, T1S, T1M},
  {NOREGION, 0, 0},
  {CHICAGOLAWN, T2S, T2M},
  {GAGEPARK, T1S, T1M},
  {WESTELSDON, T1S, T1M},
  {ARCHERHEIGHTS, T2S, T2M},
  {NOREGION, 0, 0},
  {ARCHERHEIGHTS, T3S, T3M},
  {BRIGHTONPARK, T1S, T1M},
  {SOUTHLAWNDALE, T2S, T2M},

  {SOUTHLAWNDALE, T3S, T3M},
  {NORTHLAWNDALE, T3S, T3M},
  {EASTGARFIELDPARK, T1S, T1M},
  {EASTGARFIELDPARK, T2S, T2M},
  {HUMBOLDTPARK, T3S,T3M},
  {WESTTOWN, T1S, T1M},
  {NOREGION, 0, 0},
  {AVONDALE, T1S, T1M},
  {AVONDALE, T2S, T2M},
  {IRVINGPARK, T2S, T2M},
  {ALBANYPARK, T2S, T2M},
  {NOREGION, 0, 0},
  {WESTRIDGE, T1S, T1M},
  {NORTHPARK, T2S, T2M},
  {NOREGION, 0, 0},
  {WESTRIDGE, T2S, T2M},
  
  {ROGERSPARK, T1S, T1M},
  {ROGERSPARK, T2S, T2M},
  {WESTRIDGE, T3S, T3M},
  {ROGERSPARK, T3S, T3M},
  {EDGEWATER, T1S, T1M},
  {LINCOLNSQUARE, T1S, T1M},
  {EDGEWATER, T2S, T2M},
  {EDGEWATER, T3S, T3M},
  {UPTOWN, T1S, T1M},
  {UPTOWN, T2S, T2M},
  
  {LAKEVIEW, T1S, T1M},
  {UPTOWN, T3S, T3M},
  {UPTOWN, T4S, T4M},
  {LINCOLNSQUARE, T2S, T2M},
  {LAKEVIEW, T2S, T2M},
  {NORTHCENTER, T1S, T1M},
  {LINCOLNPARK, T1S, T1M},
  
  {LOGANSQUARE, T2S, T2M},
  {LOGANSQUARE, T3S, T3M},
  {WESTTOWN, T2S, T2M},
  {LOGANSQUARE, T4S, T4M},
  {WESTTOWN, T3S, T3M},
  {LINCOLNPARK, T2S, T2M},
  {NOREGION, 0, 0},
  {LAKEVIEW, T3S, T3M},
  {LAKEVIEW, T4S, T4M},

  {LINCOLNPARK, T3S, T3M},
  {LINCOLNPARK, T4S, T4M},
  {NEARNORTHSIDE, T1S, T1M},
  {NEARNORTHSIDE, T2S, T2M},
  {WESTTOWN, T4S, T4M},
  {NEARNORTHSIDE, T3S, T3M},
  {NEARNORTHSIDE, T4S, T4M},
  {NEARNORTHSIDE, T5S, T5M},
  {NEARNORTHSIDE, T6S, T6M},
  {NEARNORTHSIDE, T7S, T7M},
  {LOOP, T1S, T1M},
  {LOOP, T2S, T2M},
  {NEARWESTSIDE, T1S, T1M},
  {NEARWESTSIDE, T2S, T2M},
  {LOOP, T3S, T3M},
  {LOOP, T4S, T4M},
  {SOUTHLOOP, T1S, T1M},
  {SOUTHLOOP, T2S, T2M},
  {SOUTHLOOP, T3S, T3M},
  {SOUTHLOOP, T4S, T4M},
  {ARMOURSQUARE, T1S, T1M},
  {SOUTHLOOP, T5S, T5M},
  {DOUGLAS, T1S, T1M},
  {DOUGLAS, T2S, T2M},
  {DOUGLAS, T3S, T3M},
  {ARMOURSQUARE, T2S, T2M},
  {ARMOURSQUARE, T3S, T3M},
  {BRONZEVILLE, T1S, T1M},
  {FULLERPARK, T1S, T1M},
  {FULLERPARK, T2S, T2M},
  {BRIDGEPORT, T1S, T1M},
  {NOREGION, 0, 0},
  {NEWCITY, T1S, T1M},
  {MCKINLEYPARK, T1S, T1M},
  {BRIDGEPORT, T2S, T2M},
  {BRIDGEPORT, T3S, T3M},
  {LOWERWESTSIDE, T1S, T1M},
  {NEARWESTSIDE, T3S, T3M},
  {NEARWESTSIDE, T4S, T4M},
  {WESTTOWN, T5S, T5M},
  {NEARWESTSIDE, T5S, T5M},
  {NEARWESTSIDE, T6S, T6M},
  {LOWERWESTSIDE, T2S, T2M},
  {LOWERWESTSIDE, T3S, T3M},
  {MCKINLEYPARK, T2S, T2M},
  {NOREGION, 0, 0},
  {NEWCITY, T2S, T2M},
  {WESTENGLEWOOD, T1S, T1M},
  {NOREGION, 0, 0},
  {NEWCITY, T3S, T3M},
  {ENGLEWOOD, T1S, T1M},
  {FULLERPARK, T3S, T3M},
  {WASHINGTONPARK, T1S, T1M},
  {WASHINGTONPARK, T2S, T2M},
  {GREATERGRANDCROSSING, T1S, T1M},
  {NOREGION, 0, 0},
  {ENGLEWOOD, T2S, T2M},
  {WESTENGLEWOOD, T2S, T2M},
  {AUBURNGRESHAM, T1S, T1M},
  {NOREGION, 0, 0},
  {BEVERLY, T1S, T1M},
  {NOREGION, 0, 0},
  {MORGANPARK, T1S, T1M},
  {MORGANPARK, T2S, T2M},
  {WESTPULLMAN, T1S, T1M},
  {NOREGION, 0, 0},
  {BEVERLY, T2S, T2M},
  {WASHINGTONHEIGHTS, T1S, T1M},
  {ROSELAND, T1S, T1M},
  {WASHINGTONHEIGHTS, T2S, T2M},
  {NOREGION, 0, 0},
  {WASHINGTONHEIGHTS, T3S, T3M},
  {AUBURNGRESHAM, T2S, T2M},
  {CHATHAM, T1S, T1M},
  {CHATHAM, T2S, T2M},
  {GREATERGRANDCROSSING, T2S, T2M},
  {GREATERGRANDCROSSING, T3S, T3M},
  {WOODLAWN, T1S, T1M},
  {WOODLAWN, T2S, T2M},
  {HYDEPARK, T1S, T1M},
  {BRONZEVILLE, T1S, T1M},
  {KENWOOD, T1S, T1M},
  {OAKLAND, T1S, T1M},
  {OAKLAND, T2S, T2M},
  {KENWOOD, T2S, T2M},
  {KENWOOD, T3S, T3M},
  {HYDEPARK, T2S, T2M},
  {HYDEPARK, T3S, T3M},
  {WOODLAWN, T3S, T3M},
  {SOUTHSHORE, T1S, T1M},
  {SOUTHSHORE, T2S, T2M},
  {AVALONPARK, T1S, T1M},
  {NOREGION, 0, 0},
  {AVALONPARK, T2S, T2M},
  {CALUMETHEIGHTS, T1S, T1M},
  {BURNSIDE, T1S, T1M},
  {PULLMAN, T1S, T1M},
  {ROSELAND, T2S, T2M},
  {ROSELAND, T3S, T3M},
  {PULLMAN, T2S, T2M},
  {NOREGION, 0, 0},
  {SOUTHDEERING, T1S, T1M},
  {NOREGION, 0, 0},
  {CALUMETHEIGHTS, T2S, T2M},
  {SOUTHCHICAGO, T1S, T1M},
  {NOREGION, 0, 0},
  {SOUTHSHORE, T3S, T3M},
  {SOUTHCHICAGO, T2S, T2M},
  {EASTSIDE, T1S, T1M},
  {EASTSIDE, T2S, T2M},
  {EASTSIDE, T3S, T3M},
  {SOUTHDEERING, T2S, T2M},
  {NOREGION, 0, 0},
  {SOUTHDEERING, T3S, T3M},
  {NOREGION, 0, 0},
  {WESTPULLMAN, T2S, T2M},
  {WESTPULLMAN, T3S, T3M},
  {NOREGION, 0, 0},
  {RIVERDALE, T1S, T1M},
  {RIVERDALE, T2S, T2M},
  {NOREGION, 0, 0},
  {HEGEWISCH, T1S, T1M},
  {NOREGION, 0, 0},
  {HEGEWISCH, T2S, T2M}};
  


// This function is hardcoded and must be updated to match the panel definition.
// It prints all of the current values by region, for all existing regions.
void printButtonPressesByRegion(){
  Serial.println("All recorded data:");
  for (int i = 0; i < numButtons; i++){
    booReadyToPrint = true;
    switch(i){
      /*
      case REGION1:
        Serial.print("REGION1, "); break;
      case REGION2:
        Serial.print("REGION2, "); break;
      case REGION3:
        Serial.print("REGION3, "); break;
      case REGION4:
        Serial.print("REGION4, "); break;
        */
      default:
        booReadyToPrint = false;
        break;
    }
    if (booReadyToPrint == true){Serial.print(lngButtonCounter[i]);Serial.print(",");Serial.println(lngButtonDisplayCounter[i]);}
  }
}

#endif
