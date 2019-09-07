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
#include <ADC.h>

void setup() {
  delay(1000); // give power rails time to settle- we don't want to erroniously call a write cycle
  // before the program has even started because we think everything is shutting down.
  pinMode(pinLEDDATA, OUTPUT);
  Serial.begin(9600);
  // initializing hardware pins (and default states where necessary).
  initializePortExpanders();
  loadStateFromDisk();
  leds.begin();
  colorWipe(RED,50);
  leds.clear();
  leds.show();
  initializeTimers();
  updateLEDs(); // establish the known state. probably not needed since this happens right away in the loop,
  // but not harmful either.
}

void loop() {
  checkButtons(); // scan all buttons
  interpretButtons(); // if they've been pressed, increment counters as needed
  interpretSerialCommands(); // if someone is requesting a change or information, process it now
  checkTiming(); // some functions only happen at rare intervals. check the timers and deal with them now.
  updateLEDs(); // all graphic animation computation is done here, and the LEDs are printed.
  checkInputPower();
  delay(30); // set the refresh rate to roughly 30hz. The button handler is counting on this (and the low CPU usage of this
  // project will keep it roughly right), so don't mess with this or add any delays elsewhere unless you are ok with the buttons
  // not responding.
}

// saves all presets to disk. Could be more efficient and only save the ones that have changed, or swap up the address used
// to prevent wear, but for this project we try to just not do it too often.
int EEPROMPOINTER = 0;
void saveStateToDisk(){
  EEPROMPOINTER = 0;
  for (int i = 0; i < numButtons; i++){
    EEPROM.put(EEPROMPOINTER, lngButtonCounter[i]);
    EEPROMPOINTER += 4;
    EEPROM.put(EEPROMPOINTER, lngButtonDisplayCounter[i]);
    EEPROMPOINTER += 4;
  }
  Serial.println("All button counts saved to disk.");
}

// loads all presets from disk. Useful on startup. A total hack but it works.
void loadStateFromDisk(){
    EEPROMPOINTER = 0;
  for (int i = 0; i < numButtons; i++){
    EEPROM.get(EEPROMPOINTER, lngButtonCounter[i]);
    EEPROMPOINTER += 4;
    EEPROM.get(EEPROMPOINTER, lngButtonDisplayCounter[i]);
    EEPROMPOINTER += 4;
  }
  Serial.println("All button counts loaded from disk");
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
  for (int counter = 0; counter < numButtons; counter ++){
    lngButtonLastPressedTime[counter] = millis() - ((maxBlinkDuration*1000)+100);
  }
}

// check to see if enough time has passed to perform rare actions like saving and
// overflow protection
void checkTiming(){

}

// if input voltage is too low, we may have been unplugged and are running on capacitance.
// save all data, then hold and wait to see if power levels increase. apply significant 
// hysterysis so that a noisy power rail does not cause rapid save cycles
void checkInputPower(){
  
  if (analogRead(pinINPUTVOLTAGESENSE) < 198){ // 7V
    saveStateToDisk();
    Serial.println("Voltage VERY low, saving and halting until problem is corrected.");
    while(analogRead(pinINPUTVOLTAGESENSE) < 282){ // 10V
      Serial.println("ERROR, POWER TOO LOW");
      Serial.println(analogRead(pinINPUTVOLTAGESENSE));
      // turn all lights to blue to let user know there has been an error, but also to make
      // sure that the lights weren't what was dragging down the rails (ie, the lights turn off,
      // then the rail bounces back up, rinse and repeat.)
      for (int counter = 0; counter < numled; counter ++){
        leds.setPixel(counter, BLUE);
      }
      leds.show();
      delay(100); // keep serial buffer and LED buffer from going crazy
    }
  }
}
