#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <stdlib.h>  // For random number generation
#include <time.h>    // For seeding random number generation

// TFT display pin definitions (ESP32-WROOM)
#define TFT_DC    12 // Data/Command pin
#define TFT_CS    13 // Chip Select pin
#define TFT_MOSI  14 // Master Out Slave In pin
#define TFT_CLK   27 // Clock pin
#define TFT_RST    4 // Reset pin

// Initialize TFT display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void setup() {
  Serial.begin(115200);
  
  // Initialize the TFT display
  tft.initR(INITR_BLACKTAB);  // Adjust the initialization as needed
  tft.setRotation(1);  // Set rotation (1 is landscape mode)
  tft.fillScreen(ST7735_WHITE);  // Fill the screen with white

  drawTable();
  fillFirstColumn();
  addHeader();
  fillSecondColumn();  // Add random percentage values to the second column
}

void loop() {
  // Nothing to do here for now
}

// Function to draw a 2-column, 10-row table (1 header + 9 rows)
void drawTable() {
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  
  // Define cell size based on the screen width and height
  int cellWidth = screenWidth / 2;
  int cellHeight = screenHeight / 10;

  // Set the color for lines
  uint16_t lineColor = ST7735_BLACK;

  // Draw horizontal lines (10 rows including header)
  for (int i = 1; i <= 10; i++) {
    int y = i * cellHeight;
    tft.drawLine(0, y, screenWidth, y, lineColor);
  }

  // Draw vertical line for columns (skipping the first row)
  for (int i = 1; i < 2; i++) {
    int x = i * cellWidth;
    tft.drawLine(x, cellHeight, x, screenHeight, lineColor);
  }
}

// Function to fill the first column with "Sensor1", "Sensor2", ..., "Sensor9"
void fillFirstColumn() {
  int screenHeight = tft.height();
  int cellHeight = screenHeight / 10;

  tft.setTextColor(ST7735_BLUE);  // Set text color
  tft.setTextSize(1);  // Set text size (1 is default size)

  // Loop to print "Sensor1", "Sensor2", ..., "Sensor9"
  for (int i = 1; i <= 9; i++) {
    int textX = 5;  // X position to start text (left margin)
    int textY = (i * cellHeight) + (cellHeight / 2) - 4;  // Y position for each row (centered vertically)

    tft.setCursor(textX, textY);
    tft.print("Sensor");
    tft.print(i);
  }
}

// Function to add a header "IOT sensor value" in the first row
void addHeader() {
  int screenWidth = tft.width();  // Get screen width
  int screenHeight = tft.height(); // Get screen height
  int cellHeight = screenHeight / 10; // Calculate cell height

  tft.setTextColor(ST7735_RED);  // Set text color
  tft.setTextSize(1);  // Set text size (1 is default size)

  // Set cursor to the center of the first row
  tft.setCursor((screenWidth / 2) - 40, (cellHeight / 2) - 4);  // Adjust these values to center the text

  // Print the header text
  tft.print("IOT sensor value");
}

// Function to fill the second column with random percentage values
void fillSecondColumn() {
  int screenHeight = tft.height();
  int cellHeight = screenHeight / 10;

  // Seed the random number generator
  srand(time(NULL));

  // Loop to print random percentage values in the second column
  for (int i = 1; i <= 9; i++) {
    int textX = tft.width() / 2 + 5;  // X position to start text in the second column
    int textY = (i * cellHeight) + (cellHeight / 2) - 4;  // Y position for each row (centered vertically)

    int percentage = rand() % 101;  // Generate a random percentage between 0 and 100

    // Set text color based on the percentage value
    if (percentage < 60) {
      tft.setTextColor(ST7735_RED);  // Set text color to red if value is less than 60%
    } else {
      tft.setTextColor(ST7735_BLACK);  // Set text color to black otherwise
    }

    tft.setCursor(textX, textY);
    tft.print(percentage);
    tft.print("%");
  }
}
