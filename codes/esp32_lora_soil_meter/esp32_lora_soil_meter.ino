#include <LoRa.h>
#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>
#include <math.h>  // For trigonometric functions

// Pin definitions for Arduino Mega (LoRa)
#define LORA_NSS 5   // NSS pin
#define LORA_RST 2   // Reset pin
#define LORA_DIO0 -1 // DIO0 pin (not used)

// Pin definitions for ESP32-WROOM (TFT Display)
#define TFT_DC 12     // Data/Command pin
#define TFT_CS 13     // Chip Select pin
#define TFT_MOSI 14   // MOSI pin
#define TFT_CLK 27    // Clock pin
#define TFT_RST 0     // Reset pin
#define TFT_MISO 0    // MISO pin (not used)

// Create TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Meter dimensions
#define METER_RADIUS  30    // Adjusted radius for a 1.8-inch display
#define METER_CENTER_X 80   // Center x-coordinate for the meter
#define METER_CENTER_Y 70   // Center y-coordinate for the meter
#define VALUE_Y_OFFSET 105  // Vertical offset for the value text

void setup() {
  // Initialize serial communication for debugging purposes
  Serial.begin(115200);
  while (!Serial);

  // Initialize LoRa
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);  // Set the pins for LoRa
  if (!LoRa.begin(433E6)) {  // Initialize LoRa with the frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Receiver Initialized");

  // Initialize the display
  tft.initR(INITR_BLACKTAB);  // Initialize with ST7735 display type
  tft.setRotation(1);         // Set to landscape orientation
  tft.fillScreen(ST7735_BLACK);  // Fill screen with black color
  tft.setTextColor(ST7735_RED);  // Set text color to red
  tft.setTextSize(1);            // Set text size for the header
  tft.setCursor(0, 0);           // Set cursor position
  tft.println("LoRa Receiver");  // Initial message on the TFT display
}

void loop() {
  // Check for received LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = "";
    while (LoRa.available()) {
      packet += (char)LoRa.read();  // Read packet data
    }

    // Assume packet format: "Sensor1:Value1"
    int rssi = LoRa.packetRssi();  // Get RSSI value

    // Output received packet to Serial Monitor
    Serial.print("Received packet: ");
    Serial.print(packet);
    Serial.print(" with RSSI: ");
    Serial.println(rssi);

    // Parse packet data (assuming a format with "Sensor1:Value1")
    String sensorData = packet.substring(packet.indexOf("Sensor1:") + 8);

    // Convert sensor data to integer (assumed to be between 0 and 100)
    int sensorValue = sensorData.toInt();

    // Update the TFT display with the received packet in a graphical meter format
    tft.fillScreen(ST7735_BLACK);  // Clear screen

    // Draw Meter
    drawRoundMeter(sensorValue);
  }

  // Add any other functionality here
}

void drawRoundMeter(int value) {
  // Draw meter background
  tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS, ST7735_WHITE);
  
  // Draw meter border
  tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS + 1, ST7735_WHITE);

  // Draw filled arc (meter) based on sensor value
  int startAngle = -90; // Start angle (top of the meter)
  int endAngle = map(value, 0, 100, 0, 180); // End angle based on sensor value

  for (int angle = startAngle; angle <= endAngle; angle++) {
    float angleRad = radians(angle); // Convert angle to radians
    int x = METER_CENTER_X + METER_RADIUS * cos(angleRad);
    int y = METER_CENTER_Y + METER_RADIUS * sin(angleRad);
    tft.drawLine(METER_CENTER_X, METER_CENTER_Y, x, y, ST7735_GREEN);
  }
  tft.setCursor(0,0);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);  // Adjusted text size
  tft.print("Sensor1");
  // Draw value text below the meter
  tft.setCursor(0, VALUE_Y_OFFSET);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);  // Adjusted text size
  tft.print("Value: ");
  tft.print(value);

}
