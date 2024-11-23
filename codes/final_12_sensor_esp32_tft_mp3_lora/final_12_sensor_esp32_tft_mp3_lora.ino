#include <LoRa.h>
#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>
#include <math.h>  // For trigonometric functions
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

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

// Push button pin definition
#define BUTTON_PIN 15  // Change to the actual pin connected to your button

// Create TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Meter dimensions
#define METER_RADIUS  30    // Adjusted radius for a 1.8-inch display
#define METER_CENTER_X 80   // Center x-coordinate for the meter
#define METER_CENTER_Y 70   // Center y-coordinate for the meter
#define VALUE_Y_OFFSET 105  // Vertical offset for the value text

// Sensor data storage for 12 sensors
int sensorValues[12] = {0};  // Array to store sensor values
int currentSensor = 1;       // Start with sensor 1
bool buttonPressed = false;

// Initialize hardware serial on pins 16 (RX2) and 17 (TX2)
HardwareSerial myHardwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  // Initialize serial communication for debugging purposes
  Serial.begin(115200);
  //while (!Serial);

  // Initialize button input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LoRa
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);  // Set the pins for LoRa
  if (!LoRa.begin(433E6)) {  // Initialize LoRa with the frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Receiver Initialized");

  // Initialize the display
  tft.initR(INITR_BLACKTAB);  // Initialize with ST7735 display type
  tft.setRotation(3);  // Set to upside-down orientation
  tft.fillScreen(ST7735_BLACK);  // Fill screen with black color
  tft.setTextColor(ST7735_RED);  // Set text color to red
  tft.setTextSize(2);            // Set text size for the header
  tft.setCursor(0, 0);           // Set cursor position
  tft.println("LoRa Receiver");  // Initial message on the TFT display

  // Initialize DFPlayer Mini module
  myHardwareSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(myHardwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }

  Serial.println(F("DFPlayer Mini module initialized!"));
  myDFPlayer.volume(30);  // Set volume level (0-30)
}

void loop() {
  // Check for received LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = "";
    while (LoRa.available()) {
      packet += (char)LoRa.read();  // Read packet data
    }

    // Output received packet to Serial Monitor
    Serial.print("Received packet: ");
    Serial.println(packet);

    // Parse packet data for all 12 sensors
    for (int i = 0; i < 12; i++) {
      String sensorLabel = "Sensor" + String(i + 1) + ":";
      int sensorStartIndex = packet.indexOf(sensorLabel);
      if (sensorStartIndex != -1) {
        sensorValues[i] = packet.substring(sensorStartIndex + sensorLabel.length(), packet.indexOf(';', sensorStartIndex)).toInt();
      }
    }

    // Display the sensor value on the TFT display
    displaySensorValue();
  }

  // Check for button press to switch sensors
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      currentSensor = (currentSensor % 12) + 1;  // Cycle through sensors 1 to 12
      buttonPressed = true;
      displaySensorValue();  // Update the display to show the new sensor value
    }
  } else {
    buttonPressed = false;  // Reset button press state
  }
}

void displaySensorValue() {
  // Clear the screen and display the meter for the current sensor
  tft.fillScreen(ST7735_BLACK);

  String sensorName = "Sensor" + String(currentSensor);
  drawRoundMeter(sensorValues[currentSensor - 1], sensorName);

  // Play the corresponding audio track if the sensor value is less than 50
  if (sensorValues[currentSensor - 1] < 50) {
    myDFPlayer.play(currentSensor);  // Play track corresponding to the current sensor
  }
}

void drawRoundMeter(int value, String sensorName) {
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

  // Draw sensor name at the top
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);  // Adjusted text size
  tft.print(sensorName);

  // Draw value text below the meter
  tft.setCursor(0, VALUE_Y_OFFSET);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);  // Adjusted text size
  tft.print("Value: ");
  tft.print(value);
  tft.print("%");
}
