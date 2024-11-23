#include <LoRa.h>
#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>

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
  tft.setTextSize(2);            // Set text size
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

    // Assume packet format: "Sensor1:Value1;Sensor2:Value2;Sensor3:Value3"
    int rssi = LoRa.packetRssi();  // Get RSSI value

    // Output received packet to Serial Monitor
    Serial.print("Received packet: ");
    Serial.print(packet);
    Serial.print(" with RSSI: ");
    Serial.println(rssi);

    // Parse packet data (assuming a specific format with semicolon separators)
    String sensor1Data = packet.substring(packet.indexOf("Sensor1:") + 8, packet.indexOf(';', packet.indexOf("Sensor1:")));
    String sensor2Data = packet.substring(packet.indexOf("Sensor2:") + 8, packet.indexOf(';', packet.indexOf("Sensor2:")));
    String sensor3Data = packet.substring(packet.indexOf("Sensor3:") + 8, packet.indexOf(';', packet.indexOf("Sensor3:")));

    // Update the TFT display with the received packet in table format
    tft.fillScreen(ST7735_BLACK);  // Clear screen

    // Row 1 - Sensor1
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_GREEN);
    tft.print("Sensor1: ");
    tft.setTextColor(ST7735_WHITE);
    tft.println(sensor1Data);

    // Row 2 - Sensor2
    tft.setCursor(0, 30);
    tft.setTextColor(ST7735_GREEN);
    tft.print("Sensor2: ");
    tft.setTextColor(ST7735_WHITE);
    tft.println(sensor2Data);

    // Row 3 - Sensor3
    tft.setCursor(0, 60);
    tft.setTextColor(ST7735_GREEN);
    tft.print("Sensor3: ");
    tft.setTextColor(ST7735_WHITE);
    tft.println(sensor3Data);
  }

  // Add any other functionality here
}
