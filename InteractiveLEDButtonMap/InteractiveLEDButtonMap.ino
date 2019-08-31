

// Kyle Mayer
// 8/28/2019

// HARDCODED PANEL DEFINITIONS:
const int numled = 20; 
// First, define all the regions by absolute button number. There MUST NOT be repeats, or the code will not compile
// (it wouldn't make much sense to have multiple regions recorded with a single button, if that is the case rename the region
// even if only for the software.
#define NOREGION -1
#define REGION1 47
#define REGION2 42
#define REGION3 32
#define REGION4 33
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
long LEDLookupTable[numled][3]={
  {0,0,0}, // LED0
  {0,0,0}, // LED1
  {0,0,0}, // LED2
  {0,0,0}, // LED3
  {0,0,0}, // LED4
  {0,0,0}, // LED5
  {0,0,0}, // LED6
  {REGION1,199,867}, // LED7
  {0,0,0}, // LED8
  {0,0,0}, // LED9
  {0,0,0}, // LED10
  {0,0,0}, // LED11
  {0,0,0}, // LED12
  {0,0,0}, // LED13
  {0,0,0}, // LED14
  {0,0,0}, // LED15
  {0,0,0}, // LED16
  {0,0,0}, // LED17
  {0,0,0}, // LED18
  {0,0,0}}; // LED19
  
// LIBRARY INCLUDES
#include <SPI.h>              // We use this library within the MCP23S17 library, so it must be called here.
#include <MCP23S17.h>
#include <EEPROM.h>
#include <WS2812Serial.h>

// COLOR DEFINITIONS AND GAMMA CORRECTION
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
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

// PIN DECLARATIONS
const unsigned short pinHEARTBEAT = 9;
const unsigned short pinCS_BUTTONS = 10;
const unsigned short pinLEDDATA = 1;

// VARIABLE DECLARATIONS

// ANIMATION CONTROL VARIABLES
int minBlinkSpeed = 3000; // max time in ms. that an LED will take to fade from on to off and back to on. after this time is reached,
// the time held at on will begin to increase. once the time held on reaches maximum, the LED will convert to always on (ie, not blinking)
int maxBlinkSpeed = 500; // min time in ms. that an LED will take to fade from on to off and back to on. when blinkDuration is
// at 0, this is the maximum speed at which the LED will blink.
int maxBlinkHold = 1000; // max time in ms. that an LED will stay on between blinks before just converting to always on
int maxBlinkDuration = 1000; // max time in seconds that an LED will blink for after the corresponding button is pressed.

// RANDOM HELPER VARIABLES
int heartBeatCounter = 0; // only for dividing down timer2 to a visible speed
boolean heartBeatStatus = false; // keeps track of heartbeat on or off.
int mask = 0; // helper variable for reading mcp registers or doing basic mask work
boolean booReadyToPrint = false; // helper variable for printing all button data
SPISettings mySetting(1000,MSBFIRST,SPI_MODE0); // we want to go very slow, since our SPI devices may be a ways away

// BUTTON VARIABLES
#define numButtons 128 // this is the maximum number of supported buttons, not neccissarily the actual number of buttons.
// currently this MUST be 128 or other things may not work normally.
// keeps track of the status of buttons for debouncing and timing (for functions
// that respond to how long a button has been held for.) This one actually does need to be an array, since the value is used for timing.
int intButtonStatus[numButtons] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
int intButtonValue = 0; // helper variable for more efficient communication with mcp port expanders
int intButtonPointer = 0; // helper variable for indexing into the button array
long lngButtonCounter[numButtons]; // a counter that keeps track of how many times each button has been pressed since the beginning of time
long lngButtonDisplayCounter[numButtons]; // same as the last counter, but this one's values may decay over time so that the animations don't max out.
long lngButtonLastPressedTime[numButtons]; // time in ms that the button was last pressed. important for blink decay algorithms

// fancy variables for LEDs
byte rawDrawingMemory[numled*3];         //  3 bytes per LED
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
WS2812Serial leds(numled, displayMemory, drawingMemory, pinLEDDATA, WS2812_RGB);


// PORT EXPANDERS
// there might be a cleaner way to do this with an array or a list or a new object type
// also, we may not always have all the drivers populated, and there is some wasted overhead scanning
// them all, but for this project we have nothing else to do with our processing time so who cares.
MCP mcpButtons0  (0, pinCS_BUTTONS);
MCP mcpButtons1  (1, pinCS_BUTTONS);
MCP mcpButtons2  (2, pinCS_BUTTONS);
MCP mcpButtons3  (3, pinCS_BUTTONS);
MCP mcpButtons4  (4, pinCS_BUTTONS);
MCP mcpButtons5  (5, pinCS_BUTTONS);
MCP mcpButtons6  (6, pinCS_BUTTONS);
MCP mcpButtons7  (7, pinCS_BUTTONS);

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
  //Serial.println("Example look up table entry: ");
  //Serial.print(LEDLookupTable[7][0]);Serial.print(",");Serial.print(LEDLookupTable[7][1]);Serial.print(",");Serial.println(LEDLookupTable[7][2]);
  //Serial.println();
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
// ways of doing this and existing libraries to manage it, but this is pretty simple and hardware specific.
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


// HARDWARE SPECIFIC FUNCTION
// This function decides what a button press means, and activates the solenoids as needed,
// and updates the LEDs to match.
// this function may be a blocking function, as if any buttons have been pressed it may actuate the solenoids,
// and currently our solenoid drive is blocking. Depending on the button pressed (like the default buttons for 
// example), this function may need to actuate a great deal of solenoids and block for a long time.
// It is hardware specific because many buttons have specific, special functions.
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
int displayCount = 0;
void updateLEDs(){
  for (int i = 0; i < numled; i++){
    intDisplayCount = lngButtonDisplayCount[LEDLookupTable[i][0]];
    lngLastPress = lng[LEDLookupTable[i][0]];
    targetHue = // based on number of button presses and LED scaling factors
    blinkIncrement = // based on time since last press and the target hue
    // also need to handle time on here
    // finally, if time on is 0 and blink increment is not 0, increment the actual color
    // set time on if we have reached max hue.
  }
}

// saves all presets to disk. Could be more efficient and only save the ones that have changed, or swap up the address used
// to prevent wear, but for this project we try to just not do it too often.
void saveStateToDisk(){
  Serial.println("All data saved to disk.");
}

// loads all presets from disk. Useful on startup. A total hack but it works.
void loadStateFromDisk(){

}

// basic display function to test LEDs
void colorWipe(int color, int wait) {
  for (int i=0; i < numled; i++) {
    leds.setPixel(i, color);
    leds.show();
    delay(wait);
  }
}

// sets all permanantly recorded values to zero
void resetButtonPresses(){
  for (int i = 0; i < numButtons; i++){
    lngButtonCounter[i] = 0;
    lngButtonDisplayCounter[i] = 0;
  }
  Serial.println("All internal metrics wiped, set to 0.");
}

// This function is hardcoded and must be updated to match the panel definition.
// It prints all of the current values by region, for all existing regions.
void printButtonPressesByRegion(){
  Serial.println("All recorded data:");
  for (int i = 0; i < numButtons; i++){
    booReadyToPrint = false;
    switch(i){
      case REGION1:
        Serial.print("REGION1, "); booReadyToPrint = true; break;
      case REGION2:
        Serial.print("REGION2, "); booReadyToPrint = true; break;
      case REGION3:
        Serial.print("REGION3, "); booReadyToPrint = true; break;
      case REGION4:
        Serial.print("REGION4, "); booReadyToPrint = true; break;
      default:
        break;
    }
    if (booReadyToPrint == true){Serial.print(lngButtonCounter[i]);Serial.print(",");Serial.println(lngButtonDisplayCounter[i]);}
  }
}

char serialCommand = 0;
boolean booDevMode = false;
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
  }
  while(Serial.available()>0){Serial.read();} // clear the buffer, we only respond to single characters

}
     
void initializeTimers(){
  
}

void checkTiming(){
  
}
