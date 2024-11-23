#include <SPI.h>
#include <LoRa.h>

// Pin definitions for Arduino Mega (LoRa)
#define LORA_NSS 53   // NSS pin
#define LORA_RST 23   // Reset pin
#define LORA_DIO0 -1  // DIO0 pin (not used)

// Relay pin definition
#define RELAY_PIN 7   // Pin connected to the relay

// Array for soil moisture sensor pins
const int soilMoisturePins[12] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11};
int soilMoisturePercent[12]; // Array to store moisture percentage values

void setup() {
  Serial.begin(9600);

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Initially turn off relay (assuming active LOW)

  // Initialize LoRa module
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {  // Initialize LoRa with the frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Transmitter Initialized");
}

void loop() {
  int totalMoisture = 0;  // Variable to store the sum of all sensor readings

  // Read and convert soil moisture sensor values for all 12 sensors
  for (int i = 0; i < 12; i++) {
    int soilMoistureValue = analogRead(soilMoisturePins[i]);
    soilMoisturePercent[i] = map(soilMoistureValue, 0, 1023, 100, 0);
    totalMoisture += soilMoisturePercent[i];  // Add to the total for averaging
  }

  // Calculate the average soil moisture
  int averageMoisture = totalMoisture / 12;

  // Turn on the relay if the average soil moisture is less than 50%
  if (averageMoisture < 50) {
    digitalWrite(RELAY_PIN, LOW);  // Turn on relay (assuming active LOW)
    Serial.println("Relay ON - Average moisture < 50%");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Turn off relay
    Serial.println("Relay OFF - Average moisture >= 50%");
  }

  // Send the soil moisture data as a packet
  LoRa.beginPacket();
  for (int i = 0; i < 12; i++) {
    LoRa.print("Sensor");
    LoRa.print(i + 1);
    LoRa.print(":");
    LoRa.print(soilMoisturePercent[i]);
    if (i < 11) {
      LoRa.print(";");
    }
  }
  LoRa.endPacket();

  // Print the sent packet data and average moisture to the Serial Monitor
  Serial.print("Packet sent with soil moisture percentages: ");
  for (int i = 0; i < 12; i++) {
    Serial.print("Sensor");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(soilMoisturePercent[i]);
    Serial.print("%");
    if (i < 11) {
      Serial.print(", ");
    }
  }
  Serial.print(" | Average moisture: ");
  Serial.print(averageMoisture);
  Serial.println("%");

  delay(5000); // Wait 5 seconds before sending the next packet
}
