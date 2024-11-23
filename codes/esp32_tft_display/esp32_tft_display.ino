#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
 
// These pins will also work for the 1.8" TFT shield
 
//ESP32-WROOM
#define TFT_DC 12 //A0
#define TFT_CS 13 //CS
#define TFT_MOSI 14 //SDA
#define TFT_CLK 27 //SCK
#define TFT_RST 0 
#define TFT_MISO 0
 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void setup() {
  // Initialize serial communication for debugging purposes
  Serial.begin(115200);
  
  // Initialize the display
  tft.initR(INITR_BLACKTAB);  // Initialize with ST7735 display type
  
  // Set rotation
  tft.setRotation(1);  // Landscape orientation
  
  // Fill screen with black color
  tft.fillScreen(ST7735_BLACK);
  
  // Draw a test message
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Hello, ESP32!");
}

void loop() {
  // Put your main code here, to run repeatedly:
}
