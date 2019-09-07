
#ifndef GlobalVariables_h
#define GlobalVariables_h

#include "Arduino.h"
#include "HardwareDefinitions.h"
#include <SPI.h>
#include <MCP23S17.h>
#include <EEPROM.h>
#include <WS2812Serial.h> 

static const int numled = 20;

// PIN DECLARATIONS
const unsigned short pinHEARTBEAT = 9;
const unsigned short pinCS_BUTTONS = 10;
const unsigned short pinLEDDATA = 1;
const unsigned short pinINPUTVOLTAGESENSE = 14;

// VARIABLE DECLARATIONS

// RANDOM HELPER VARIABLES
int heartBeatCounter = 0; // only for dividing down timer2 to a visible speed
boolean heartBeatStatus = false; // keeps track of heartbeat on or off.
int mask = 0; // helper variable for reading mcp registers or doing basic mask work
boolean booReadyToPrint = false; // helper variable for printing all button data
SPISettings mySetting(1000,MSBFIRST,SPI_MODE0); // we want to go very slow, since our SPI devices may be a ways away

// FANCY VARIABLES FOR LEDs
byte rawDrawingMemory[numled*3];         //  3 bytes per LED, for doing non-destructive math before gamma correction
byte drawingMemory[numled*3];         //  3 bytes per LED, actual data mask
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED, unpacked data mask for non-blocking serial print
WS2812Serial leds(numled, displayMemory, drawingMemory, pinLEDDATA, WS2812_RGB);
int blinkDimmer[numled]; // 2 bytes per LED, counter for each LED that counts from 0 to 1000 and back again that acts as an intensity
// multiplier so each led can do totally asynchronous fading blinking.
int ledAnimationState[numled]; // 2 bytes per LED (could be a byte instead, or even packed into smaller data struct), records
// whether an led is increasing in intensity or dimming, ie, which direction should blinkDimmer be moving. 1=dimming, 2=getting brighter.
// this comes out to 22 bytes per LED

// ANIMATION HELPER VARIABLES
int intDisplayCount = 0; // unpacker for the array recording number of button presses. this is the number of button presses
// that we want to display for the current LED.
int targetRed = 0; // the shade of green a given LED would be if it wasn't blinking
int targetGreen = 0; // the shade of red a given LED would be if it wasn't blinking
int intScaleRate = 0; // unpacker for the LED definition array, just the current LED's number of button presses that
// are required to reach full brightness red
unsigned long lngLastPress = 0; // an unpacker for the button pressed history so we don't have to keep indexing into an array.
unsigned long timeNow = 0; // the current millis(); just saves having to call it multiple times (and getting slightly different results).
int currentBlinkSpeed = 0; // number of ms a single dimming or brightening cycle should take, given the amount of elapsed
// time since the last button press.
int deltaBlinkDimmer = 0; // how many ms the blinkDimmer should change per frame. due to integer math there is a lot
// of rounding error here, but since blink times are not super specific, it doesn't really matter.

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
unsigned long lngButtonLastPressedTime[numButtons]; // time in ms that the button was last pressed. important for blink decay algorithms
// this comes out to 14 bytes per button (but requires that all 128 buttons get assigned anyways, so a flat 1792 bytes)

// SERIAL COMMS HELPER VARIABLES
char serialCommand = 0;
boolean booDevMode = false;

#endif
