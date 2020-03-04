// need to handle overflow from millis and math

// Kyle Mayer
// 8/28/2019

// LIBRARY INCLUDES
#include <WS2812Serial.h>

// COLOR DEFINITIONS AND GAMMA CORRECTION
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400

const unsigned short pinLEDDATA = 1;

static const int numled = 200;

// FANCY VARIABLES FOR LEDs
byte rawDrawingMemory[numled*3];         //  3 bytes per LED, for doing non-destructive math before gamma correction
byte drawingMemory[numled*3];         //  3 bytes per LED, actual data mask
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED, unpacked data mask for non-blocking serial print
WS2812Serial leds(numled, displayMemory, drawingMemory, pinLEDDATA, WS2812_RGB);

void setup() {
  pinMode(pinLEDDATA, OUTPUT);
  Serial.begin(9600);
  leds.begin();
  colorWipe(RED,100);
  delay(10000);
  leds.clear();
  leds.show();
}

void loop() {

}

// basic display function to test LEDs
void colorWipe(int color, int wait) {
  for (int i=0; i < numled; i++) {
    leds.setPixel(i, color);
    leds.show();
    delay(wait);
  }
}
