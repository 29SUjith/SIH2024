#include <esp_now.h>
#include <WiFi.h>

#define MOISTURE_SENSOR_1_PIN 33
#define MOISTURE_SENSOR_2_PIN 34

typedef struct struct_message {
    int moisture1;
    int moisture2;
} struct_message;

struct_message soilData1;
struct_message soilData2;
int localMoisture1;
int localMoisture2;

uint8_t sender1Address[] = {0xD0, 0xEF, 0x76, 0x48, 0x0F, 0x34};  // Sender 1 MAC address
uint8_t sender2Address[] = {0xB0, 0xA7, 0x32, 0x15, 0xFD, 0x6C};  // Sender 2 MAC address

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register sender 1 peer
  esp_now_peer_info_t peerInfo1;
  memcpy(peerInfo1.peer_addr, sender1Address, 6);
  peerInfo1.channel = 0;
  peerInfo1.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
    Serial.println("Failed to add peer 1");
    return;
  }

  // Register sender 2 peer
  esp_now_peer_info_t peerInfo2;
  memcpy(peerInfo2.peer_addr, sender2Address, 6);
  peerInfo2.channel = 0;
  peerInfo2.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
    Serial.println("Failed to add peer 2");
    return;
  }

  // Register callback function to handle incoming data
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // Read local soil moisture data
  localMoisture1 = analogRead(MOISTURE_SENSOR_1_PIN);
  localMoisture2 = analogRead(MOISTURE_SENSOR_2_PIN);

  // Print all soil moisture values
  Serial.print("Sender 1 | Soil 1: ");
  Serial.print(soilData1.moisture1);
  Serial.print(" | Soil 2: ");
  Serial.print(soilData1.moisture2);

  Serial.print(" | Sender 2 | Soil 1: ");
  Serial.print(soilData2.moisture1);
  Serial.print(" | Soil 2: ");
  Serial.print(soilData2.moisture2);

  Serial.print(" | Receiver | Soil 1: ");
  Serial.print(localMoisture1);
  Serial.print(" | Soil 2: ");
  Serial.println(localMoisture2);

  delay(2000);  // Update every 2 seconds
}

// Updated callback function to handle received data with metadata
void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (memcmp(info->src_addr, sender1Address, 6) == 0) {
    memcpy(&soilData1, incomingData, sizeof(soilData1));
  } else if (memcmp(info->src_addr, sender2Address, 6) == 0) {
    memcpy(&soilData2, incomingData, sizeof(soilData2));
  }
}
