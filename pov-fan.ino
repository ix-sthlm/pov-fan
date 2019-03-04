// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif


#define LED_PIN 27
#define IDX_PIN 4

const char* ssid = "POVFAN";
const char* password = "POVFAN";

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

class POVMatrix : public Adafruit_NeoMatrix {
  public:
  void showslice(int offset, int size);
  POVMatrix(int w, int h, uint8_t pin = 6,
    uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS,
    neoPixelType ledType = NEO_GRB + NEO_KHZ800)
    : Adafruit_NeoMatrix(w, h, pin, matrixType, ledType) {
    }
};

void POVMatrix::showslice(int offset, int size) {
    uint8_t *oldPixels = pixels;
    uint16_t oldnumBytes = numBytes;
    const uint8_t bytesperpixel = ((wOffset == rOffset) ? 3 : 4);
    // Horrifying abuse: trick show() into drawing a part of the buffer
    numBytes -= width()*height()*bytesperpixel - size * bytesperpixel;
    pixels += offset * bytesperpixel;
    show();
    pixels = oldPixels;
    numBytes = oldnumBytes;
}

// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
const uint16_t width = 64, height=2*8;
POVMatrix matrix = POVMatrix(width, height, LED_PIN,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  WiFi.softAP(ssid, password);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(255);
  matrix.setTextColor(colors[1]);
  ArduinoOTA.setHostname("myesp32");
  ArduinoOTA.setPassword("admin");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      //Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      //Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      /*
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
      */
    });

  ArduinoOTA.begin();

  // Enable index pin input
  pinMode(IDX_PIN, INPUT);
  //timer = timerBegin(0, 80, true);
  //timerAttachInterrupt(timer, &onTimer, true);

  // TODO: Add an update command or something
  matrix.fillScreen(matrix.Color(0,0,0));
  matrix.setCursor(0, 8);
  matrix.print(F("HELLO WORLD!"));

}

int x    = 0;
int pass = 0;

void loop() {
  ArduinoOTA.handle();
  if(--x < 0) {
    x = width-1;
    // Await index (active low)
    while (!digitalRead(IDX_PIN)) {
       //ArduinoOTA.handle();
    }
    while (digitalRead(IDX_PIN)) {
       //ArduinoOTA.handle();
    }
    //x = matrix.width();
    //if(++pass >= 3) pass = 0;
    //matrix.setTextColor(colors[pass]);
  }
  matrix.showslice(x*height, height);
  //delay(1);
  delayMicroseconds(10);
}
