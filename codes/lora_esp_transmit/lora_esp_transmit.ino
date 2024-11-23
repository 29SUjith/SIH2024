#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(115200); // ESP32 typically uses a higher baud rate
  while (!Serial);

  // Configure LoRa pins (you can adjust these pins according to your wiring)
  LoRa.setPins(5, 17, 16); // NSS, Reset, DIO0 (adjust these pins as needed)
  
  if (!LoRa.begin(865E6)) { // Frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.println("LoRa Transmitter Initialized");
}

void loop() {
  // Send a packet
  LoRa.beginPacket();
  LoRa.print("He"); // Payload data
  LoRa.endPacket();
  
  Serial.println("Packet sent");
  
  delay(1000); // Wait a second before sending next packet
}
