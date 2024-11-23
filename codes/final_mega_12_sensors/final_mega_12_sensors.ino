#include <SPI.h>
#include <LoRa.h>

// Pin definitions for Arduino Mega (LoRa)
#define LORA_NSS 53   // NSS pin
#define LORA_RST 6   // Reset pin
#define LORA_DIO0 -1  // DIO0 pin (not used)

// Array for soil moisture sensor pins
const int soilMoisturePins[12] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11};
int soilMoisturePercent[12]; // Array to store moisture percentage values

void setup() {
  Serial.begin(9600);

  // Initialize LoRa module
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {  // Initialize LoRa with the frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Transmitter Initialized");
}

void loop() {
  // Read and convert soil moisture sensor values for all 12 sensors
  for (int i = 0; i < 12; i++) {
    int soilMoistureValue = analogRead(soilMoisturePins[i]);
    soilMoisturePercent[i] = map(soilMoistureValue, 0, 1023, 100, 0);
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

  // Print the sent packet data to the Serial Monitor
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
  Serial.println();

  delay(5000); // Wait 5 seconds before sending the next packet
}