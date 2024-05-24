/*
 Project: BEAPER Analog Bar Graph     Activity: mirobo.tech/beaper
 Date:    May 24, 2024
 
 BEAPER Nano sample program demonstrating reading analog inputs and
 displaying outpu values using bar graphs on the TFT LCD display.
 This program is a work in progress and is likely to change.
*/

// Include files for TFT LCD support
#include "Adafruit_GFX.h"     // Core graphics library
#include "Adafruit_ST7789.h"  // ST7789 display controller library

// Font files
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSansBold24pt7b.h"

// BEAPER I/O (input/output) pin settings
// Define BEAPER pins used for human interface devices
const int SW2 = 0;            // Pushbuttons
const int SW3 = 1;
const int SW4 = 2;
const int SW5 = 3;

const int LED2 = 4;           // LEDs
const int LED3 = 5;
const int LED4 = 6;
const int LED5 = 7;

const int BEEPER = 8;         // Piezo beeper LS1

// Define BEAPER Analog input devices
const int ANLS = A0;          // Ambient light sensor Q4
const int ANQ4 = A0;
const int ANQ1 = A0;          // Left floor sensor module phototransistor
const int ANFL = A0;          // ANFL = AN(alog) F(loor) L(eft)
const int ANTS = A1;          // Temperature sensor U4
const int ANU4 = A1;
const int ANQ2 = A1;          // Right floor sensor module left line phototransistor
const int ANLL  = A1;         // ANLL = AN(alog) L(ine) L(eft)
const int ANPOT_L = A2;       // Left potentiometer RV1
const int ANRV1 = A2;
const int ANQ3 = A3;          // Right floor sensor module right line/floor phototransistor
const int ANFR = A3;          // ANFR = AN(alog) F(loor) R(ight)
const int ANLR = A3;          // ANLR = AN(alog) L(ine) R(ight)
const int ANPOT_R = A3;       // Right potentiometer RV2
const int ANRV2 = A3;
const int ANVDIV = A3;        // Battery voltage divider 

// Pre-defined Arduino Nano ESP32 LEDS (listed here only for name reference)
// LED_BUILTIN (D13)          // Yellow LED
// LED_BLUE                   // RGB LED blue element (active LOW)
// LED_GREEN                  // RGB LED green element (active LOW)
// LED_RED                    // RGB LED red element(acive LOW)

// Define TFT LCD SPI interface pins
const int TFT_BL = 12;        // TFT backlight control set to unused TFT CIPO pin
const int TFT_CS = 10;        // TFT CS pin
const int TFT_DC = 9;         // TFT Data/Command pin

// Pre-define LCD colours using 16-bit RGB-5-6-5 format
#define MIROBO_BLUE 0x121C
#define MEDIUM_GREY 0x4228
#define LIGHT_GREY 0x632C
#define DARK_GREY 0x2104
#define BLACK 0x0000
#define RED 0xE000
#define GREEN 0x0700
#define YELLOW 0xC600
#define BLUE 0x001C

// Connect the ST7789 display controller as 'lcd' on SPI bus
Adafruit_ST7789 lcd(TFT_CS, TFT_DC, -1);  // TFT RES pin connected to Nano RST pin
// Define a 16-bit colour, 240x240 pixel canvas memory buffer
GFXcanvas16 canvas(240, 240);

int light_level;      // Raw light level from ambient light sensor
int raw_temp;         // Raw temperature sensor voltage level
int pot_left;         // Raw left potentiometer level
int pot_right;        // Raw right potentiometer level
int intensity = 1;    // LED intensity

void setup(void) {
  Serial.begin(115200);       // Start serial port

  // Initialize I/O pin directions/types
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(BEEPER, OUTPUT);

  // Initialize 240x240 TFT LCD using SPI Mode 3
  lcd.init(240, 240, SPI_MODE3);
  lcd.fillScreen(0x0000); // Clear screen
  lcd.setRotation(1);     // Set screen rotation to 90 degrees

  pinMode(TFT_BL, OUTPUT);   // ** Testing ** - configure BL output after SPI setup
  digitalWrite(TFT_BL, HIGH); // Backlight on

  canvas.setFont(&FreeSansBold12pt7b);  // Set default typeface
  canvas.setTextColor(MIROBO_BLUE);
  canvas.setTextWrap(false);  // Clip text within canvas

  // Draw the mirobo logo on the canvas
  draw_miroboLogo();
  // Draw text on the canvas
  canvas.print("   BEAPER Nano");
  // Display the canvas on the LCD
  lcd.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
  // Wait a bit while displaying logo and to allow Serial to start
  delay(2000);

  // Serial port should be open by now
  Serial.println(F("Starting BEAPER Nano"));
}

void loop() {
  // Read analog inputs 
  light_level = map(analogRead(ANLS), 0 , 4095, 0, 255);
  raw_temp = map(analogRead(ANTS), 0, 4095, 0, 255);
  pot_left = map(analogRead(ANPOT_L), 0, 4095, 0, 255);
  pot_right = map(analogRead(ANPOT_R), 0, 4095, 0, 255);

  // Display values on TFT LCD as bar graph gauges
  // Clear canvas
  canvas.fillScreen(BLACK);
  // Draw analog gauges
  draw_gauge(0, light_level, YELLOW);
  draw_gauge(1, raw_temp, RED);
  draw_gauge(2, pot_left, GREEN);
  draw_gauge(3, pot_right, MIROBO_BLUE);
  lcd.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());

  // PWM LEDs using analog values
  analogWrite(LED2, light_level);
  analogWrite(LED3, raw_temp);
  analogWrite(LED4, pot_left);
  analogWrite(LED5, pot_right);

  // Add a short delay
  delay(16);
}

// Draws the mirobo.tech logo on the canvas while leaving room for a text status line.
void draw_miroboLogo() { 
  canvas.fillScreen(BLACK);
  canvas.fillRoundRect(0, 0, 240, 240, 20, 0xFFFF);
  canvas.fillRoundRect(28, 70, 40, 120, 20, MIROBO_BLUE);
  canvas.fillRoundRect(76, 50, 40, 140, 20, MIROBO_BLUE);
  canvas.fillRoundRect(124, 70, 40, 120, 20, MIROBO_BLUE);
  canvas.fillRoundRect(172, 100, 40, 90, 20, MIROBO_BLUE);
  canvas.fillCircle(192, 70, 20, MIROBO_BLUE);
  canvas.setCursor(10,232);   // Status line text coordinates
}

// Draws one of four full-height bar graph + number gauges using semi-dynamic
// scaling (for possible future modification?), but really designed for 60 px
// width with 6 px bar graph gauge frame width.
//
// Usage: draw_gauge(position[0-3], display-value[0-255], display-color[RGB-5-6-5])
void draw_gauge(unsigned int pos, unsigned int val, unsigned int col) {
  const int ga_width = 60;
  const int ga_frame = 6;
  const int ga_gaps = 2;
  int ga_xFrm = pos * ga_width + ga_gaps / 2;
  int ga_xLED = ga_xFrm + ga_frame;
  int ga_wLED = ga_width - ga_gaps - ga_frame * 2;
  canvas.fillRoundRect(ga_xFrm, 0, ga_width - ga_gaps, 240, 10, DARK_GREY);
  canvas.fillRect(ga_xLED, ga_frame, ga_wLED, 192, BLACK);
  int ga_LEDs = map(val, 0, 255, 0, 16);
  for(unsigned int led = 0; led < 16; led ++) {
    if(led <= ga_LEDs && val != 0) {
      canvas.fillRect(ga_xLED, 181 + ga_frame - led * 12, ga_wLED, 10, col);
    }
  }
  canvas.setFont(&FreeSans12pt7b);
  canvas.fillRect(ga_xLED, 206, ga_wLED, 28, BLACK);
  canvas.setCursor(ga_xLED + 4, 228);
  if(val < 10) {
    canvas.print("00");
  }
  else if(val < 100) {
    canvas.print("0");
  }
  canvas.print(val);
}
