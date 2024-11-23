#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pin definitions for LoRa and TFT display
#define LORA_NSS 5
#define LORA_RST 2
#define LORA_DIO0 -1

#define TFT_DC 12
#define TFT_CS 13
#define TFT_MOSI 14
#define TFT_CLK 27
#define TFT_RST 0

#define BUTTON_PIN 15

// Create TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Meter dimensions
#define METER_RADIUS 30
#define METER_CENTER_X 80
#define METER_CENTER_Y 70
#define VALUE_Y_OFFSET 105

// Sensor data storage
int sensorValues[3] = {0};  // Change to 3 sensors
int currentSensor = 1;  // Add this line to declare and initialize currentSensor
bool buttonPressed = false;

// Initialize hardware serial for DFPlayer Mini
HardwareSerial myHardwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize LoRa
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    // Initialize the display
    tft.initR(INITR_BLACKTAB);  // Initialize with ST7735 display type
    tft.setRotation(3);  // Set to upside-down orientation
    tft.fillScreen(ST7735_BLACK);  // Fill screen with black color
    tft.setTextColor(ST7735_RED);  // Set text color to red
    tft.setTextSize(2);            // Set text size for the header
    tft.setCursor(0, 0);           // Set cursor position
    tft.println("LoRa Receiver");  // Initial message on the TFT display

    // Initialize DFPlayer Mini
    myHardwareSerial.begin(9600, SERIAL_8N1, 16, 17);
    if (!myDFPlayer.begin(myHardwareSerial)) {
        Serial.println("DFPlayer Mini initialization failed");
        while (1);
    }
    myDFPlayer.volume(30);
}

void loop() {
    // Check for received LoRa packets
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String packet = "";
        while (LoRa.available()) {
            packet += (char)LoRa.read();
        }
        Serial.print("Received packet: ");
        Serial.println(packet);

        // Parse sensor values and display them
        parseSensorData(packet);
        displaySensorValue();
    }

    // Handle button press with debouncing
    handleButtonPress();
}

void displaySensorValue() {
    // Clear the screen and display the meter for the current sensor
    tft.fillScreen(ST7735_BLACK);

    // Debug: Check if the current sensor value is properly set for the third sensor
    Serial.print("Current Sensor: ");
    Serial.println(currentSensor);

    String sensorName = "Sensor" + String(currentSensor);
    drawRoundMeter(sensorValues[currentSensor - 1], sensorName);

    // Debug: Print out the sensor value for the current sensor
    Serial.print(sensorName);
    Serial.print(" Value: ");
    Serial.print(sensorValues[currentSensor - 1]);
    Serial.println("%");

    if (sensorValues[currentSensor - 1] < 50) {
        myDFPlayer.play(currentSensor);
    }
}

void drawRoundMeter(int value, String sensorName) {
    // Draw meter background
    tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS, ST7735_WHITE);
    tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS + 1, ST7735_WHITE);

    // Draw filled arc based on sensor value
    int startAngle = -90;
    int endAngle = map(value, 0, 100, 0, 180);
    for (int angle = startAngle; angle <= endAngle; angle++) {
        float angleRad = radians(angle);
        int x = METER_CENTER_X + METER_RADIUS * cos(angleRad);
        int y = METER_CENTER_Y + METER_RADIUS * sin(angleRad);
        tft.drawLine(METER_CENTER_X, METER_CENTER_Y, x, y, ST7735_GREEN);
    }

    // Draw sensor name and value text
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_YELLOW);
    tft.setTextSize(2);
    tft.print(sensorName);

    tft.setCursor(0, VALUE_Y_OFFSET);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.print("Value: ");
    tft.print(value);
    tft.print("%");
}

void parseSensorData(String packet) {
  // Expected packet format: "Sender 1 | Soil 1: X% | Sender 2 | Soil 1: Y% | Receiver | Soil 1: Z%"
  int sensorIndex = 0;
  
  // Define the labels for each sensor
  String sensorLabels[] = {"Sender 1 | Soil 1: ", "Sender 2 | Soil 1: ", "Receiver | Soil 1: "};
  
  // Extract each sensor value from the packet
  for (int i = 0; i < 3; i++) {
    int startIndex = packet.indexOf(sensorLabels[i]);
    if (startIndex != -1) {
      int endIndex = packet.indexOf('%', startIndex);
      if (endIndex != -1) {
        sensorValues[i] = packet.substring(startIndex + sensorLabels[i].length(), endIndex).toInt();
        // Debug: Print each sensor value after parsing
        Serial.print("Parsed Sensor ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(sensorValues[i]);
        Serial.println("%");
      } else {
        Serial.println("Error parsing sensor value");
      }
    } else {
      Serial.print("Error finding sensor label: ");
      Serial.println(sensorLabels[i]);
    }
  }
}

void handleButtonPress() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!buttonPressed) {
            Serial.print("Before button press: currentSensor = ");
            Serial.println(currentSensor);

            currentSensor = (currentSensor % 3) + 1;  // Update for 3 sensors

            Serial.print("After button press: Switched to Sensor ");
            Serial.println(currentSensor);

            buttonPressed = true;
            displaySensorValue();
        }
    } else {
        buttonPressed = false;
    }
}
