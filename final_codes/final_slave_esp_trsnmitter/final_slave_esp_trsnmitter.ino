#include <esp_now.h>
#include <WiFi.h>

#define MOISTURE_SENSOR_1_PIN 34


typedef struct struct_message {
    int moisture1;
} struct_message;

struct_message soilData;
uint8_t receiverAddress[] = {0xCC, 0x7B, 0x5C, 0xFD, 0x39, 0xC8};  // Receiver MAC address

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receiver peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read soil moisture data
  soilData.moisture1 = analogRead(MOISTURE_SENSOR_1_PIN);


  // Send soil moisture data to receiver
  esp_now_send(receiverAddress, (uint8_t *) &soilData, sizeof(soilData));

  // Print data for debugging
  Serial.print("Soil 1: ");
  Serial.println(soilData.moisture1);

  delay(2000);  // Send data every 2 seconds
}
