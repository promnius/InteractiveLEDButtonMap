// need to handle overflow from millis and math

// Kyle Mayer
// 8/28/2019

// LIBRARY INCLUDES
#include "HardwareDefinitions.h"
#include "LEDAnimations.h"
#include "GlobalVariables.h"
#include "ButtonHandling.h"
#include <SPI.h>              // We use this library within the MCP23S17 library, so it must be called here.
#include <MCP23S17.h>
#include <EEPROM.h>
#include <WS2812Serial.h>

void setup() {
  pinMode(pinLEDDATA, OUTPUT);
  Serial.begin(9600);
  // initializing hardware pins (and default states where necessary).
  initializePortExpanders();
  loadStateFromDisk();
  leds.begin();
  colorWipe(RED,200);
  leds.clear();
  leds.show();
  initializeTimers();
  updateLEDs(); // establish the known state.
}

void loop() {
  checkButtons(); // scan all buttons
  interpretButtons(); // if they've been pressed, increment counters as needed
  interpretSerialCommands(); // if someone is requesting a change or information, process it now
  checkTiming(); // some functions only happen at rare intervals. check the timers and deal with them now.
  updateLEDs(); // all graphic animation computation is done here, and the LEDs are printed.
  delay(30); // set the refresh rate to roughly 30hz. The button handler is counting on this (and the low CPU usage of this
  // project will keep it roughly right), so don't mess with this or add any delays elsewhere unless you are ok with the buttons
  // not responding.
}

// saves all presets to disk. Could be more efficient and only save the ones that have changed, or swap up the address used
// to prevent wear, but for this project we try to just not do it too often.
void saveStateToDisk(){
  Serial.println("All data saved to disk.");
}

// loads all presets from disk. Useful on startup. A total hack but it works.
void loadStateFromDisk(){

}

// sets all permanantly recorded values to zero
void resetButtonPresses(){
  for (int i = 0; i < numButtons; i++){
    lngButtonCounter[i] = 0;
    lngButtonDisplayCounter[i] = 0;
  }
  Serial.println("All internal metrics wiped, set to 0.");
}

// check the serial port, interpret any inputs (truncate to a single character),
// and perform any required actions
void interpretSerialCommands(){
  if (Serial.available() > 0){
    serialCommand = Serial.read();
    //Serial.print("I Read: "); Serial.println(serialCommand);
    Serial.println(); // create some white space
    if(serialCommand == '?'){
      printButtonPressesByRegion();
    }
    else if (serialCommand == 'R'){ // total system reset, irreversable
      resetButtonPresses();
    }
    else if (serialCommand == 'S'){ // save command, before power down
      saveStateToDisk();
    }
    else if (serialCommand == 'D'){ // toggle dev mode
      booDevMode = !booDevMode;
      if (booDevMode == true){Serial.println("Development mode now active");}
      else{Serial.println("Development mode turned off");}
    }
    else if (serialCommand == 'L'){ // scan lights for initial indexing purposes
      leds.clear();
      leds.show();
      delay(3000);
      colorWipe(RED,3000);
    }
  }
  while(Serial.available()>0){Serial.read();} // clear the buffer, we only respond to single characters

}

// when we start, we don't want anything blinking, so all timers need a value
void initializeTimers(){
  
}

// check to see if enough time has passed to perform rare actions like saving.
void checkTiming(){
  
}
