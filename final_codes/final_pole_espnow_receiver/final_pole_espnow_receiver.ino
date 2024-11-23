#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials and server details
const char *ssid = "Project";
const char *password = "12345678";
const char *server = "https://nvs-krishi-pragya.onrender.com/api/core/sensors/";
const String apiKey = "pk-nvs-=13ade28046bc46b78a2b336d05132910=";

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
int currentSensor = 1;      // Add this line to declare and initialize currentSensor
bool buttonPressed = false;

// Initialize hardware serial for DFPlayer Mini
HardwareSerial myHardwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize Wi-Fi
    WiFi.begin(ssid, password);
    connectToWiFi();

    // Initialize LoRa
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    // Initialize the display
    tft.initR(INITR_BLACKTAB);  // Initialize with ST7735 display type
    tft.setRotation(3);         // Set to upside-down orientation
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
        sendDataToServer();
        displaySensorValue();
    }

    // Handle button press with debouncing
    handleButtonPress();
}

void displaySensorValue() {
    // Clear the screen and display the meter for the current sensor
    tft.fillScreen(ST7735_BLACK);

    String sensorName = "Sensor" + String(currentSensor);
    drawRoundMeter(sensorValues[currentSensor - 1], sensorName);

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
            }
        }
    }
}

void handleButtonPress() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!buttonPressed) {
            currentSensor = (currentSensor % 3) + 1;  // Update for 3 sensors
            buttonPressed = true;
            displaySensorValue();
        }
    } else {
        buttonPressed = false;
    }
}

void sendDataToServer() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(server);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "DeviceToken " + apiKey);

        // Prepare the JSON document for 3 sensors
        StaticJsonDocument<256> doc;
        JsonObject soilMoisture = doc.createNestedObject("soil_moisture");

        // Add sensor values to the JSON object
        for (int i = 0; i < 3; i++) {
            soilMoisture["s" + String(i + 1)] = sensorValues[i];
        }

        String requestBody;
        serializeJson(doc, requestBody);

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void connectToWiFi() {
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}
