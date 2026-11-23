#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void setup() {
  tft.init();
  tft.setRotation(1); // Landscape mode (1 or 3)
  
  // Clear screen to black
  tft.fillScreen(TFT_BLACK);
  
  // Set text color (foreground, background) and size
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  
  // Center text roughly on the screen
  tft.setCursor(20, 55);
  tft.println("Hello World!");
}

void loop() {
  // Nothing needed here for static text
}
