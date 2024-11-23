#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Initialize hardware serial on pins 16 (RX2) and 17 (TX2)
HardwareSerial myHardwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  // Begin Serial communication with DFPlayer Mini module on hardware serial port 2
  myHardwareSerial.begin(9600, SERIAL_8N1, 16, 17);
  
  // Initialize the standard serial for debugging
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(myHardwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }

  Serial.println(F("DFPlayer Mini module initialized!"));

  // Set initial volume and play track 1
  myDFPlayer.volume(20);  // Set volume level (0-30)
  myDFPlayer.play(1);     // Play track 1
}

void loop() {
  // Nothing to do here
}
