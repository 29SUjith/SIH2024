//lora receiver
#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(5, 2, -1); // NSS, Reset, DIO0 for Arduino Mega
  
  if (!LoRa.begin(433E6)) { // Frequency for India
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.println("LoRa Receiver Initialized");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = "";
    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }
    int rssi = LoRa.packetRssi();
    Serial.print("Received packet: ");
    Serial.print(packet);
    Serial.print(" with RSSI: ");
    Serial.println(rssi);
  }
}
