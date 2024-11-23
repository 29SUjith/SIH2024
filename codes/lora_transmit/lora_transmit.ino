//lora transmitter

#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  LoRa.setPins(53, 11, -1); // NSS, Reset, DIO0 (DIO0 not used)
  
  if (!LoRa.begin(433E6)) { // Frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.println("LoRa Transmitter Initialized");
}

void loop() {
  // Send a packet
  LoRa.beginPacket();
  LoRa.print("SIH"); // Payload data
  LoRa.endPacket();
  
  Serial.println("Packet sent");
  
  delay(1000); // Wait a second before sending next packet
}
