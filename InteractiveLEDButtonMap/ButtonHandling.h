
#ifndef ButtonHandling_h
#define ButtonHandling_h

#include "Arduino.h"
#include "HardwareDefinitions.h"
#include "GlobalVariables.h"

// set up all the port expanders for beginning their journey, to be called once.
void initializePortExpanders(){  
  SPI.begin();
  SPI.beginTransaction(mySetting);
  mcpButtons0.begin();
  mcpButtons1.begin();
  mcpButtons2.begin();
  mcpButtons3.begin();
  mcpButtons4.begin();
  mcpButtons5.begin();
  mcpButtons6.begin();
  mcpButtons7.begin();

  SPI.beginTransaction(mySetting); // not sure if mcp.begin overwrites my settings or not
            
  mcpButtons0.pinMode(0xFFFF);     // Use word-write mode to set all of the pins on inputchip to be inputs
  mcpButtons0.pullupMode(0xFFFF);  // Use word-write mode to Turn on the internal pull-up resistors.
  mcpButtons0.inputInvert(0xFFFF); // Use word-write mode to invert the inputs so that logic 0 is read as HIGH
  mcpButtons1.pinMode(0xFFFF);
  mcpButtons1.pullupMode(0xFFFF);
  mcpButtons1.inputInvert(0xFFFF);
  mcpButtons2.pinMode(0xFFFF);
  mcpButtons2.pullupMode(0xFFFF);
  mcpButtons2.inputInvert(0xFFFF);
  mcpButtons3.pinMode(0xFFFF);
  mcpButtons3.pullupMode(0xFFFF);
  mcpButtons3.inputInvert(0xFFFF);
  mcpButtons4.pinMode(0xFFFF);
  mcpButtons4.pullupMode(0xFFFF);
  mcpButtons4.inputInvert(0xFFFF);
  mcpButtons5.pinMode(0xFFFF);
  mcpButtons5.pullupMode(0xFFFF);
  mcpButtons5.inputInvert(0xFFFF);
  mcpButtons6.pinMode(0xFFFF);
  mcpButtons6.pullupMode(0xFFFF);
  mcpButtons6.inputInvert(0xFFFF);
  mcpButtons7.pinMode(0xFFFF);
  mcpButtons7.pullupMode(0xFFFF);
  mcpButtons7.inputInvert(0xFFFF);
}

// scans the buttons and increments their counter if the button is held down. It inverts the number when the button
// is released, to tell the button interpreter that an action can be performed if the button has been held long
// enough. This provides a crude debounce
// and time the button has been held if this function is called at regular intervals. There are far better 
// ways of doing this and existing libraries to manage it, but this is pretty simple.
// it will not increment a button past 1000 to prevent overflows (who cares if it has been held down for days?)
void checkButtons(){
  for (int counter=0;counter<8;counter++){// scan through all 8 port expanders
    switch(counter){
      case 0:
        intButtonValue = mcpButtons0.digitalRead();
        break;
      case 1:
        intButtonValue = mcpButtons1.digitalRead();
        break;
      case 2:
        intButtonValue = mcpButtons2.digitalRead();
        break;
      case 3:
        intButtonValue = mcpButtons3.digitalRead();
        break;
      case 4:
        intButtonValue = mcpButtons4.digitalRead();
        break;
      case 5:
        intButtonValue = mcpButtons5.digitalRead();
        break;
      case 6:
        intButtonValue = mcpButtons6.digitalRead();
        break;
      case 7:
        intButtonValue = mcpButtons7.digitalRead();
        break;
    }    
    for (int i=0; i<16; i++){
      intButtonPointer = i + 16 * counter;
      mask = 0b0000000000000001 << i;
      if ((intButtonValue & mask) == mask){ // the button is pressed
        if (intButtonStatus[intButtonPointer] < 1){
          intButtonStatus[intButtonPointer] = 1;
        } else {
          intButtonStatus[intButtonPointer] ++; // keep track of how long the button has been pressed for
        }
        if (intButtonStatus[intButtonPointer] > 1000){ // prevent overflow
          intButtonStatus[intButtonPointer] = 1000;
        }
      }
      else { // the button is NOT pressed
        if (intButtonStatus[intButtonPointer] < 0) { // the button has been off for a cycle, if anything needed to happen it should
          // have been handled already. we can reset the button now.
          intButtonStatus[intButtonPointer] = 0;
        }
        if (intButtonStatus[intButtonPointer] > 0) { // the button has just been released, make the number negative to indicate
          // to anyone watching that the button has been released, and how long it was held for
          intButtonStatus[intButtonPointer] = 0-intButtonStatus[intButtonPointer];
        }
      }
    }
  }   
}

// if a button was just released, increment the appropriate counter.
void interpretButtons(){
  for (int i=0; i<numButtons; i++){ //
    if (intButtonStatus[i] < 0) { // button just released, could use -1 for more debounce
      Serial.print("Button "); Serial.print(i); Serial.println(" was just pressed (released)");
      lngButtonLastPressedTime[i] = millis();
      lngButtonCounter[i]++;
      lngButtonDisplayCounter[i]++;
    }
  }
}

#endif
