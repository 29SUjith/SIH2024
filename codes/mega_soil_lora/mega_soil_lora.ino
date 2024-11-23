#include <SPI.h>
#include <LoRa.h>

// Pin definitions for Arduino Mega (LoRa)
#define LORA_NSS 53   // NSS pin
#define LORA_RST 22   // Reset pin
#define LORA_DIO0 -1  // DIO0 pin (not used)

// Pins for soil moisture sensors
#define SOIL_MOISTURE_SENSOR1 A0
#define SOIL_MOISTURE_SENSOR2 A1
#define SOIL_MOISTURE_SENSOR3 A2

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
  // Read soil moisture sensor values from A0, A1, and A2
  int soilMoistureValue1 = analogRead(SOIL_MOISTURE_SENSOR1);
  int soilMoistureValue2 = analogRead(SOIL_MOISTURE_SENSOR2);
  int soilMoistureValue3 = analogRead(SOIL_MOISTURE_SENSOR3);

  // Convert the analog readings (0-1023) to percentages (0-100)
  int soilMoisturePercent1 = map(soilMoistureValue1, 0, 1023, 100, 0);
  int soilMoisturePercent2 = map(soilMoistureValue2, 0, 1023, 100, 0);
  int soilMoisturePercent3 = map(soilMoistureValue3, 0, 1023, 100, 0);

  // Send the soil moisture data as a packet
  LoRa.beginPacket();
  LoRa.print("Sensor1:");
  LoRa.print(soilMoisturePercent1);
  LoRa.print(";Sensor2:");
  LoRa.print(soilMoisturePercent2);
  LoRa.print(";Sensor3:");
  LoRa.print(soilMoisturePercent3);
  LoRa.endPacket();

  Serial.print("Packet sent with soil moisture percentages: ");
  Serial.print("Sensor1: ");
  Serial.print(soilMoisturePercent1);
  Serial.print("%, Sensor2: ");
  Serial.print(soilMoisturePercent2);
  Serial.print("%, Sensor3: ");
  Serial.print(soilMoisturePercent3);
  Serial.println("%");

  delay(5000); // Wait a second before sending the next packet
}
