/*

Timm Lines

*/

#include <MatrixHardware_Teensy4_ShieldV5.h>        // SmartLED Shield for Teensy 4 (V5)
#include <SmartMatrix.h>
#include "Vector.h"

#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 64;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 64;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_64ROW_MOD32SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

const int defaultBrightness = (100*255)/100;        // (brightness%*255)/100 
const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {0, 0, 0};

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

struct EckP
{
   PVector location;
   PVector velocity;
};

EckP lines[32][2];


void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  matrix.addLayer(&backgroundLayer); 
  matrix.begin();

  matrix.setBrightness(defaultBrightness);
  backgroundLayer.enableColorCorrection(true);

  lines[0][0] = {PVector(0, 0), PVector(-2, 1)};
  lines[0][1] = {PVector(matrix.getScreenWidth(), matrix.getScreenHeight()), PVector(3, -2)};

}


void loop() {

    // backgroundLayer.fillScreen(defaultBackgroundColor);
    
    for (int i=0; i<2; i++) {
      if(lines[0][i].location.x > matrix.getScreenWidth()) { 
        lines[0][i].location.x = matrix.getScreenWidth();
        lines[0][i].velocity.x *= -1;
      }

       if(lines[0][i].location.x < 0) { 
        lines[0][i].location.x = 0;
        lines[0][i].velocity.x *= -1;
      }

       if(lines[0][i].location.y > matrix.getScreenHeight()) { 
        lines[0][i].location.y = matrix.getScreenHeight();
        lines[0][i].velocity.y *= -1;
      }

      if(lines[0][i].location.y < 0) { 
        lines[0][i].location.y = 0;
        lines[0][i].velocity.y *= -1;
      }
    
      lines[0][i].location.x += lines[0][i].velocity.x;
      lines[0][i].location.y += lines[0][i].velocity.y;
    }
    
    rgb24 color;
    int size = sizeof (lines) / sizeof (lines[0]);  

    for (int i = size; i > 0; i--) {
    color.red = 255/size*(size-i);
    color.green = 255/size*(size-i);
    color.blue = 255/size*(size-i);
    
      backgroundLayer.drawLine(lines[i][0].location.x, lines[i][0].location.y, lines[i][1].location.x, lines[i][1].location.y, color);
      if (i != 0) {
        lines[i][0] = lines[i - 1][0];
        lines[i][1] = lines[i - 1][1];
      }
    }

    delay(50);
    backgroundLayer.swapBuffers();

}
