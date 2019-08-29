

// Kyle Mayer
// 8/28/2019


// LIBRARY INCLUDES
#include <SPI.h>              // We use this library within the MCP23S17 library, so it must be called here.
#include <MCP23S17.h>
#include <EEPROM.h>
#include <ADC.h>
#include <WS2812Serial.h>
#include <Adafruit_NeoPixel.h>

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
int heartBeatCounter = 0; // only for dividing down timer2 to a visible speed
boolean heartBeatStatus = false; // keeps track of heartbeat on or off.

// keeps track of the status of buttons for debouncing and timing (for functions
// that respond to how long a button has been held for.) This one actually does need to be an array, since the value is used for timing.
int intButtonStatus[128] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
int intButtonValue = 0; // helper variable for more efficient communication with mcp port expanders
int intButtonPointer = 0; // helper variable for indexing into the button array

// fancy variables for LEDs
const int numled = 20;
//byte drawingMemory[numled*3];         //  3 bytes per LED
//DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
//WS2812Serial leds(numled, displayMemory, drawingMemory, pinLEDDATA, WS2812_RGB);
// this library doesn't work because we have 2811 leds
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(numled, pinLEDDATA, NEO_RGB + NEO_KHZ400);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

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

  Serial.begin(9600);

  // initializing hardware pins (and default states where necessary).
  initializePortExpanders();

  loadStateFromDisk();

  //leds.begin();
  pinMode(6, INPUT); // what the actual frack like I don't even understand . . . this library breaks if this specific pin is an output . . .
  // since this is the default pin used by the library, I wonder if some buried code has it hardcoded . . .
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  //strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  //leds.setBrightness(64);
  //colorWipe(RED,1000);

  updateLEDs();
}

void loop() {
  checkButtons();
  interpretButtons(); // this will handle all the driving of the appropriate solenoids and the LEDs.
  delay(30); // set the refresh rate to roughly 30hz. The button handler is counting on this (and the low CPU usage of this
  // project will keep it roughly right), so don't mess with this or add any delays elsewhere unless you are ok with the buttons
  // not responding.
  //Serial.println("Loop!");
  Serial.println(intButtonStatus[0]);
}

// set up all the port expanders for beginning their journey, to be called once.
void initializePortExpanders(){  
  mcpButtons0.begin();
  mcpButtons1.begin();
  mcpButtons2.begin();
  mcpButtons3.begin();
  mcpButtons4.begin();
  mcpButtons5.begin();
  mcpButtons6.begin();
  mcpButtons7.begin();
            
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
      case 1:
        intButtonValue = mcpButtons1.digitalRead();
      case 2:
        intButtonValue = mcpButtons2.digitalRead();
      case 3:
        intButtonValue = mcpButtons3.digitalRead();
      case 4:
        intButtonValue = mcpButtons4.digitalRead();
      case 5:
        intButtonValue = mcpButtons5.digitalRead();
      case 6:
        intButtonValue = mcpButtons6.digitalRead();
      case 7:
        intButtonValue = mcpButtons7.digitalRead();
    }
    for (int i=0; i<16; i++){
      intButtonPointer = i + 8 * counter;
      if (intButtonValue&(0b0000000000000001 << i) == 1){ // the button is pressed
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
  for (int i=0; i<128; i++){ // the first 13 buttons just toggle turnout states when pressed (when released)
    if (intButtonStatus[i] < 0) { // button just released, could use -1 for more debounce
      Serial.print("Button "); Serial.print(i); Serial.println(" was pressed (released)");
    }
    else if (intButtonStatus[i] > 100) {
      Serial.print("Button "); Serial.print(i); Serial.println(" is being held");
    }
  }
}




// HARDWARE SPECIFIC FUNCTION
// Currently, this function updates the LEDs to match the status held in the intSolenoidStatus register.
// it doesn't actually modify the solenoids, so if a change is made to the register a command to modify the 
// solenoid must be sent separately.
// it is hardware specific because certain LEDs have real world significance, so we are not just matching LED0 to
// solenoid0, etc.
// there could be a more intelligent way to do this . . . only updating the LEDs that changed, and only calling
// this when some LEDs changed, but this is simple too.
// could also be more efficient by building a mask and talking to the port expanders once.
void updateLEDs(){

}

// saves all presets to disk. Could be more efficient and only save the ones that have changed, or swap up the address used
// to prevent wear, but for this project we try to just not do it too often.
void saveStateToDisk(){

}

// loads all presets from disk. Useful on startup. A total hack but it works.
void loadStateFromDisk(){

}

// basic display function to test LEDs
void colorWipe(int color, int wait) {
  for (int i=0; i < numled; i++) {
    strip.setPixelColor(i, 255,255,0);
    strip.show();
    delay(wait);
  }
}
