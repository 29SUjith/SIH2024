#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>

// Pin definitions for ESP32 (LoRa)
#define LORA_NSS 5    // NSS pin
#define LORA_RST 2    // Reset pin
#define LORA_DIO0 -1  // DIO0 pin (not used)

// Moisture sensor pins (local)
#define MOISTURE_SENSOR_PIN 33  // Keep only one local moisture sensor

// Structure to store moisture data from ESP-NOW senders
typedef struct struct_message {
    int moisture1;
    int moisture2;
} struct_message;

struct_message soilData1; // Data from sender 1
struct_message soilData2; // Data from sender 2
int localMoisture1;

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

  // Initialize LoRa
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {  // Initialize LoRa with 433 MHz frequency
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa and ESP-NOW Initialized");
}

void loop() {
  // Read and convert local soil moisture data to percentage (0-100%)
  localMoisture1 = map(analogRead(MOISTURE_SENSOR_PIN), 0, 4095, 100, 0);

  // Print all soil moisture values as percentages
  Serial.print("Sender 1 | Soil 1: ");
  Serial.print(soilData1.moisture1);
  Serial.print("% | ");

  Serial.print("Sender 2 | Soil 1: ");
  Serial.print(soilData2.moisture1);
  Serial.print("% | ");

  Serial.print("Receiver | Soil 1: ");
  Serial.print(localMoisture1);
  Serial.println("%");

  // Transmit data over LoRa in percentage
  sendLoRaData();

  delay(2000);  // Update every 2 seconds
}

// Callback function to handle received data from ESP-NOW senders
void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (memcmp(info->src_addr, sender1Address, 6) == 0) {
    memcpy(&soilData1, incomingData, sizeof(soilData1));
    soilData1.moisture1 = map(soilData1.moisture1, 0, 4095, 100, 0); // Convert to percentage
  } else if (memcmp(info->src_addr, sender2Address, 6) == 0) {
    memcpy(&soilData2, incomingData, sizeof(soilData2));
    soilData2.moisture1 = map(soilData2.moisture1, 0, 4095, 100, 0); // Convert to percentage
  }
}

// Function to send soil moisture data over LoRa in percentage
void sendLoRaData() {
  LoRa.beginPacket();

  // Transmit soil moisture data from sender 1 as percentage
  LoRa.print("Sender 1 | Soil 1: ");
  LoRa.print(soilData1.moisture1);
  LoRa.print("% | ");

  // Transmit soil moisture data from sender 2 as percentage
  LoRa.print("Sender 2 | Soil 1: ");
  LoRa.print(soilData2.moisture1);
  LoRa.print("% | ");

  // Transmit local soil moisture data as percentage
  LoRa.print("Receiver | Soil 1: ");
  LoRa.print(localMoisture1);
  LoRa.print("%");

  LoRa.endPacket();
}
