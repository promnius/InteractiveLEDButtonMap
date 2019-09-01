

#ifndef LEDAnimations_h
#define LEDAnimations_h

#include "Arduino.h"
#include "HardwareDefinitions.h"
#include "GlobalVariables.h"

// COLOR DEFINITIONS AND GAMMA CORRECTION
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400

const uint8_t PROGMEM gamma8[] = {
    0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };



// a pixel setter function that records the pixel to a secondary buffer instead of 
// the drawing buffer so gamma correction can be non-destructive.
void setRawPixel(int pixel, byte red, byte green, byte blue){
  rawDrawingMemory[pixel*3] = blue;
  rawDrawingMemory[pixel*3+1] = green;
  rawDrawingMemory[pixel*3+2] = red;
}

// non-destructively gamma corrects all LEDs
void gammaCorrect(){
  for (int i = 0; i < numled*3; i++){
    drawingMemory[i] = pgm_read_byte(&gamma8[rawDrawingMemory[i]]);
    //leds.setPixel(i,rawDrawingMemory[i*3],rawDrawingMemory[i*3+1],rawDrawingMemory[i*3+2]);
  }
}

// basic display function to test LEDs
void colorWipe(int color, int wait) {
  for (int i=0; i < numled; i++) {
    leds.setPixel(i, color);
    leds.show();
    delay(wait);
  }
}

// HARDWARE SPECIFIC FUNCTION
// This function handles all animation rendering and sending data out to the LEDs.
// there are a lot of different ways to do this, but the way I have selected is to do all the animation
// math directly in place in the LED buffers (using a duplicate buffer so that gamma correction is non-
// destructive). There are a lot of disadvantages to this, including timing being imprecise, animations being
// heavily hardcoded (it could be a lot of work to modify or replace certain animations), etc. But for the 
// animations we are looking at, this is the fastest, easiest method to implement.
// In the future, consider trying to implement animations from an existing library such as ALA. I choose not
// to do that here because one of the things that is very important to my animations is the totally asyncronous
// nature of the blinking, which would require a lot of timing and callbacks with a library like that. This method
// just increments everythign by a frame every time it is called.
void updateLEDs(){
  for (int i = 0; i < numled; i++){
    if (LEDLookupTable[i][0] == NOREGION){
      targetGreen = 0;
      targetRed = 0;
    }
    else{
      intDisplayCount = lngButtonDisplayCounter[LEDLookupTable[i][0]]; // grab the value we are supposed to be displaying
      lngLastPress = lngButtonLastPressedTime[LEDLookupTable[i][0]]; // grab the time since our associated button was last pressed
      intDisplayCount = intDisplayCount - LEDLookupTable[i][2]; // adjust the value we are supposed to be displaying based on offset information
      //Serial.print("(");Serial.print(i);Serial.print(",");Serial.print(intDisplayCount);Serial.print(")");
      intScaleRate = LEDLookupTable[i][1];
      if (intScaleRate<3){ // will cause divide by zero error
        intScaleRate = 3;
      }
      if (intDisplayCount < 0){ // negative displays don't make sense
        intDisplayCount = 0;
      }
      if (intDisplayCount< (intScaleRate/3)){ // first third scales intensity, second 2 thirds scales hue
        //Serial.print(":");Serial.print("G");
        targetGreen = (255*(long)intDisplayCount)/(intScaleRate/3); // linear increase in intensity
        targetRed = 0;
      }
      else if (intDisplayCount < intScaleRate){ // second two thirds scales hue, intensity is max
        //Serial.print(":");Serial.print("R");
        intDisplayCount = intDisplayCount-(intScaleRate/3);
        targetRed = (255*(long)intDisplayCount)/((intScaleRate*2)/3); // linear shift in hue
        targetGreen = 255 - targetRed;
      }
      else{
        // beyond maximum
        targetGreen = 0;
        targetRed = 255;
      }
    }
    if (targetRed == 0 && targetGreen == 0 && intDisplayCount > 0){
      targetGreen = 1; // correct for integer rounding, make sure that
      // the first button press always provides feedback.
    }
    
    timeNow = millis();
    if (timeNow < lngLastPress){
      // handle overflow
    }
    if ((timeNow-lngLastPress)<100){ // this is a very crude way of catching the transition from stable to blinking
      ledAnimationState[i]=1;
    }
    if ((timeNow - lngLastPress)/1000 < maxBlinkDuration){
      // this is the number of ms that are used for dimming and brightening.
        //currentBlinkSpeed = ((((timeNow-lngLastPress)/1000)*(minBlinkSpeed-maxBlinkSpeed))/(maxBlinkDuration/2))+minBlinkSpeed;
      currentBlinkSpeed = ((((timeNow-lngLastPress)/1000)*(minBlinkSpeed-maxBlinkSpeed))/(maxBlinkDuration))+maxBlinkSpeed;
      deltaBlinkDimmer = (30*1000)/currentBlinkSpeed; // 30 ms is the frame rate, 1000 increases number size to reduce rounding error.
      if (ledAnimationState[i] == 1){
        blinkDimmer[i] = blinkDimmer[i] - deltaBlinkDimmer; // getting dimmer.
      }
      else if (ledAnimationState[i] == 2){ // getting brighter
        blinkDimmer[i] = blinkDimmer[i] + deltaBlinkDimmer;
      }
      if (blinkDimmer[i] < 1){
        blinkDimmer[i] = 0;
        ledAnimationState[i] = 2;
      }
      else if (blinkDimmer[i] > 999){
        blinkDimmer[i] = 1000;
        ledAnimationState[i] = 1;
      }
      if (i == 8){
        //Serial.print("blink dimmer: "); Serial.print(blinkDimmer[i]);Serial.print(" Animation State: "); Serial.println(ledAnimationState[i]);
        //Serial.print("deltaBlinkDimmer: "); Serial.print(deltaBlinkDimmer); Serial.print(" currentBlinkSpeed: ");Serial.println(currentBlinkSpeed);
      }
      setRawPixel(i, (byte)((targetRed*blinkDimmer[i])/1000),(byte)((targetGreen*blinkDimmer[i])/1000),(byte)0);
    }
    else{
      // lights are on
      setRawPixel(i,(byte)targetRed,(byte)targetGreen,(byte)0);
    }
  }
  gammaCorrect();
  //Serial.println();
  leds.show();
}




#endif
