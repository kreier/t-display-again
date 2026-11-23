/*
 * ======================================================================================
 * Project:     T-Display "Hello World!" — Russian & Vietnamese Editions
 * Platform:    LilyGO TTGO T-Display (ESP32-D0WDQ6, ST7789 240x135 IPS LCD, 4MB Flash)
 * Author:      Matthias Kreier / AI Assistant
 * License:     MIT
 *
 * Description:
 *   A feature-packed, polished "Hello World!" application with native multi-language
 *   support for both Russian ("Привет, мир!") and Vietnamese ("Xin chào thế giới!").
 *
 *   Includes:
 *     - Full Russian Cyrillic UTF-8 rendering via FreeSansBold12 & FreeSans9
 *     - Full Vietnamese UTF-8 rendering with all tone marks & diacritics
 *       via VietnameseSansBold12 & VietnameseSans9 + VietnameseEncoder
 *     - Runtime language toggle (Hold Button 2 for >1.5s)
 *     - 5 selectable color themes (Cyberpunk, Emerald Matrix, Sunset, Nordic, CRT)
 *     - Multiple interactive display modes:
 *         1. Greeting Card ("Привет, мир!" / "Xin chào thế giới!")
 *         2. System Diagnostics (ESP32 chip, CPU, 4MB Flash, Free RAM, SDK)
 *         3. Font Alphabet Showcase (Russian Cyrillic or Vietnamese with diacritics)
 *     - 180-degree display rotation toggle (Hold Button 1 for >1.5s)
 *     - Hardware button debouncing and non-blocking real-time clock counter
 *     - Serial diagnostics at 115200 baud
 *
 * Hardware Pinout (LilyGO TTGO T-Display):
 *   - TFT MOSI:    GPIO 19
 *   - TFT SCLK:    GPIO 18
 *   - TFT CS:      GPIO 5
 *   - TFT DC:      GPIO 16
 *   - TFT RST:     GPIO 23
 *   - TFT BL:      GPIO 4   (Backlight power control, active HIGH)
 *   - Button 1:    GPIO 0   (Top / Left button, active LOW, internal pull-up)
 *   - Button 2:    GPIO 35  (Bottom / Right button, active LOW, input-only pin)
 * ======================================================================================
 */

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// Russian Cyrillic Font Headers
#include "FreeSansBold12.h"
#include "FreeSans9.h"

// Vietnamese Font Headers & UTF-8 Decoder
#include "VietnameseSansBold12.h"
#include "VietnameseSans9.h"
#include "VietnameseEncoder.h"

// Hardware Pin Definitions
#define TFT_BL_PIN   4   // LCD Backlight control pin (active HIGH)
#define BTN1_PIN     0   // Left/Top button (GPIO 0, active LOW)
#define BTN2_PIN     35  // Right/Bottom button (GPIO 35, active LOW)

// TFT_eSPI instance
TFT_eSPI tft = TFT_eSPI();

// Supported Languages
enum AppLanguage {
  LANG_RU = 0, // Russian: "Привет, мир!"
  LANG_VI = 1  // Vietnamese: "Xin chào thế giới!"
};

#if defined(APP_DEFAULT_LANG_VI)
AppLanguage currentLang = LANG_VI;
#else
AppLanguage currentLang = LANG_RU;
#endif

// UI Color Theme Definition
struct Theme {
  const char* name;
  uint16_t bg;         // Main screen background
  uint16_t cardBg;     // Central card background
  uint16_t cardBorder; // Card outline border
  uint16_t primary;    // Primary headline text
  uint16_t secondary;  // Subtitle / accent text
  uint16_t headerBg;   // Top header bar background
  uint16_t headerText; // Top header text
};

// 5 Color Themes
const Theme themes[] = {
  // Cyberpunk: Deep midnight, neon cyan, magenta
  { "Cyberpunk",  0x0863, 0x0108, 0x05FD, 0x07FF, 0xF81F, 0x0167, 0xFFFF },
  // Emerald: Matrix dark, vibrant green, mint accent
  { "Emerald",    0x10A2, 0x0861, 0x04C5, 0x07E0, 0x87F0, 0x0283, 0xFFFF },
  // Sunset: Dark navy, amber gold, coral orange
  { "Sunset",     0x1084, 0x0842, 0xFA60, 0xFD20, 0xFC08, 0x2867, 0xFFFF },
  // Nordic Ice: Slate blue, crisp white, glacier cyan
  { "Nordic Ice", 0x18E5, 0x0964, 0x3CFB, 0xFFFF, 0x7E1F, 0x21EB, 0xFFFF },
  // Amber CRT: Pitch black, retro phosphor amber
  { "Amber CRT",  0x0000, 0x0000, 0xD4A0, 0xFDC0, 0xFA20, 0x2100, 0xFDE0 }
};

const uint8_t NUM_THEMES = sizeof(themes) / sizeof(themes[0]);
uint8_t currentTheme = 0;

// Screen Modes
enum DisplayScreen {
  SCREEN_GREETING = 0, // Main Greeting Card
  SCREEN_SYSINFO  = 1, // ESP32 Hardware Diagnostics
  SCREEN_ALPHABET = 2, // Font Alphabet Showcase
  SCREEN_COUNT    = 3
};
DisplayScreen currentScreen = SCREEN_GREETING;

// Display Rotation: 1 = Landscape (USB on left), 3 = Landscape (USB on right)
uint8_t currentRotation = 1;

// Timing & state tracking
unsigned long lastUptimeUpdate = 0;
unsigned long lastBtnCheck     = 0;
bool btn1LastState             = HIGH;
bool btn2LastState             = HIGH;
unsigned long btn1PressStart   = 0;
unsigned long btn2PressStart   = 0;

// ======================================================================================
// UTF-8 TO RUSSIAN CYRILLIC ENCODING HELPER
// ======================================================================================
String utf8rus(const char* source) {
  String target = "";
  int i = 0;
  int k = strlen(source);

  while (i < k) {
    unsigned char n = source[i++];
    if (n >= 127) {
      switch (n) {
        case 208: { // 0xD0 prefix
          n = source[i++];
          if (n == 129) { n = 192; } // 'Ё'
          break;
        }
        case 209: { // 0xD1 prefix
          n = source[i++];
          if (n == 145) { n = 193; } // 'ё'
          break;
        }
      }
    }
    target += (char)n;
  }
  return target;
}

// ======================================================================================
// DISPLAY RENDERING FUNCTIONS
// ======================================================================================

/**
 * @brief Draws the top navigation / status header bar.
 */
void drawHeader() {
  const Theme& th = themes[currentTheme];
  tft.fillRect(0, 0, 240, 18, th.headerBg);
  tft.drawLine(0, 18, 239, 18, th.cardBorder);
  
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(th.headerText, th.headerBg);

  // Left: Board title & Language indicator
  tft.setTextDatum(ML_DATUM);
  if (currentLang == LANG_RU) {
    tft.drawString(" TTGO T-Display [RU]", 4, 9);
  } else {
    tft.drawString(" TTGO T-Display [VI]", 4, 9);
  }

  // Right: Active theme name
  tft.setTextDatum(MR_DATUM);
  tft.drawString(th.name, 236, 9);
}

/**
 * @brief Draws the bottom footer bar with button hints and live uptime counter.
 */
void drawFooter() {
  const Theme& th = themes[currentTheme];
  tft.fillRect(0, 120, 240, 15, th.cardBg);
  tft.drawLine(0, 119, 239, 119, th.cardBorder);

  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(th.secondary, th.cardBg);

  // Left: Button hints
  tft.setTextDatum(ML_DATUM);
  if (currentScreen == SCREEN_GREETING) {
    tft.drawString(" [L]Theme [R]Info", 4, 127);
  } else if (currentScreen == SCREEN_SYSINFO) {
    tft.drawString(" [L]Theme [R]Font", 4, 127);
  } else {
    tft.drawString(" [L]Theme [R]Home", 4, 127);
  }

  // Right: Uptime in MM:SS format
  char uptimeStr[16];
  unsigned long totalSec = millis() / 1000;
  unsigned int mins = totalSec / 60;
  unsigned int secs = totalSec % 60;
  snprintf(uptimeStr, sizeof(uptimeStr), "%02u:%02u ", mins, secs);

  tft.setTextDatum(MR_DATUM);
  tft.drawString(uptimeStr, 238, 127);
}

/**
 * @brief Main Greeting Screen: Displays "Привет, мир!" or "Xin chào thế giới!".
 */
void drawGreetingScreen() {
  const Theme& th = themes[currentTheme];

  // Fill main content area
  tft.fillRect(0, 19, 240, 100, th.bg);

  // Central Card Container with rounded corners
  tft.fillRoundRect(8, 24, 224, 90, 6, th.cardBg);
  tft.drawRoundRect(8, 24, 224, 90, 6, th.cardBorder);

  // Accent Corner Decals
  tft.fillCircle(16, 32, 2, th.secondary);
  tft.fillCircle(224, 32, 2, th.secondary);

  tft.setTextDatum(MC_DATUM);

  if (currentLang == LANG_RU) {
    // Primary Headline: Russian "Привет, мир!"
    tft.setFreeFont(&FreeSansBold12pt8b);
    tft.setTextColor(th.primary, th.cardBg);
    tft.drawString(utf8rus("Привет, мир!"), 120, 55);

    // Subtitle: "Hello World!"
    tft.setFreeFont(&FreeSans9pt8b);
    tft.setTextColor(th.secondary, th.cardBg);
    tft.drawString("Hello World!", 120, 78);
  } else {
    // Primary Headline: Vietnamese "Xin chào thế giới!"
    tft.setFreeFont(&VietnameseSansBold12pt8b);
    tft.setTextColor(th.primary, th.cardBg);
    tft.drawString(utf8viet("Xin chào thế giới!"), 120, 55);

    // Subtitle: "Hello World!"
    tft.setFreeFont(&VietnameseSans9pt8b);
    tft.setTextColor(th.secondary, th.cardBg);
    tft.drawString("Hello World!", 120, 78);
  }

  // Hardware Subcaption
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(th.cardBorder, th.cardBg);
  tft.drawString("ESP32-D0WDQ6 * 4MB FLASH * ST7789", 120, 99);
}

/**
 * @brief System Diagnostics Screen: Displays ESP32 chip specs, memory, and display info.
 */
void drawSysInfoScreen() {
  const Theme& th = themes[currentTheme];
  tft.fillRect(0, 19, 240, 100, th.bg);

  tft.fillRoundRect(8, 23, 224, 92, 4, th.cardBg);
  tft.drawRoundRect(8, 23, 224, 92, 4, th.cardBorder);

  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(th.primary, th.cardBg);
  tft.setTextDatum(ML_DATUM);

  char buf[48];
  snprintf(buf, sizeof(buf), "CPU:    ESP32 @ %u MHz", (unsigned int)ESP.getCpuFreqMHz());
  tft.drawString(buf, 16, 35);

  snprintf(buf, sizeof(buf), "Flash:  %u MB (Speed: %u MHz)",
           (unsigned int)(ESP.getFlashChipSize() / (1024 * 1024)),
           (unsigned int)(ESP.getFlashChipSpeed() / 1000000));
  tft.drawString(buf, 16, 50);

  snprintf(buf, sizeof(buf), "Heap:   %u KB free / %u KB total",
           (unsigned int)(ESP.getFreeHeap() / 1024),
           (unsigned int)(ESP.getHeapSize() / 1024));
  tft.drawString(buf, 16, 65);

  snprintf(buf, sizeof(buf), "Screen: 240x135 IPS ST7789 (Rot %d)", currentRotation);
  tft.drawString(buf, 16, 80);

  const char* langStr = (currentLang == LANG_RU) ? "Russian (RU)" : "Vietnamese (VI)";
  snprintf(buf, sizeof(buf), "Lang:   %s", langStr);
  tft.drawString(buf, 16, 95);
}

/**
 * @brief Alphabet Showcase Screen: Demonstrates Cyrillic or Vietnamese character rendering.
 */
void drawAlphabetScreen() {
  const Theme& th = themes[currentTheme];
  tft.fillRect(0, 19, 240, 100, th.bg);

  tft.fillRoundRect(8, 23, 224, 92, 4, th.cardBg);
  tft.drawRoundRect(8, 23, 224, 92, 4, th.cardBorder);

  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(th.secondary, th.cardBg);
  tft.setTextDatum(TC_DATUM);

  if (currentLang == LANG_RU) {
    tft.drawString("Russian Cyrillic Alphabet Test", 120, 27);

    tft.setFreeFont(&FreeSans9pt8b);
    tft.setTextColor(th.primary, th.cardBg);
    tft.setTextDatum(MC_DATUM);

    // Line 1: Uppercase subset 1
    tft.drawString(utf8rus("АБВГДЕЁЖЗИЙКЛМ"), 120, 50);
    // Line 2: Uppercase subset 2
    tft.drawString(utf8rus("НОПРСТУФХЦЧШЩ"), 120, 70);
    // Line 3: Lowercase subset
    tft.drawString(utf8rus("абвгдеёжзийклмноп"), 120, 92);
  } else {
    tft.drawString("B\xC3\xA0ng ch\xE1\xBB\xAF c\xC3\xA1i ti\xE1\xBA\xBFng Vi\xE1\xBB\x87t", 120, 27); // "Bảng chữ cái tiếng Việt" in ASCII fallback or draw with font

    tft.setFreeFont(&VietnameseSans9pt8b);
    tft.setTextColor(th.primary, th.cardBg);
    tft.setTextDatum(MC_DATUM);

    // Line 1: Vietnamese Vowels 1
    tft.drawString(utf8viet("a à á ả ã ạ  ă ằ ắ ẳ ẵ ặ"), 120, 48);
    // Line 2: Vietnamese Vowels 2
    tft.drawString(utf8viet("â ầ ấ ẩ ẫ ậ  e è é ẻ ẽ ẹ"), 120, 68);
    // Line 3: Vietnamese Vowels 3
    tft.drawString(utf8viet("ê ề ế ể ễ ệ  ơ ờ ớ ở ỡ ợ"), 120, 88);
  }
}

/**
 * @brief Fully redraws the active screen view.
 */
void redrawCurrentScreen() {
  drawHeader();
  switch (currentScreen) {
    case SCREEN_GREETING:
      drawGreetingScreen();
      break;
    case SCREEN_SYSINFO:
      drawSysInfoScreen();
      break;
    case SCREEN_ALPHABET:
      drawAlphabetScreen();
      break;
    default:
      break;
  }
  drawFooter();
}

// ======================================================================================
// ARDUINO SETUP
// ======================================================================================
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);

  Serial.println("\n========================================================");
  Serial.println("   LilyGO TTGO T-Display: Russian & Vietnamese Edition");
  Serial.printf("   Active Language: %s\n", (currentLang == LANG_RU) ? "Russian (RU)" : "Vietnamese (VI)");
  Serial.printf("   ESP32 Chip: %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("   Flash Size: %u MB\n", (unsigned int)(ESP.getFlashChipSize() / (1024 * 1024)));
  Serial.println("========================================================");

  // Configure Button Pins
  pinMode(BTN1_PIN, INPUT_PULLUP); // Button 1 has internal pull-up
  pinMode(BTN2_PIN, INPUT);        // Button 2 has on-board pull-up

  // Power on ST7789 Backlight (GPIO 4 is active HIGH)
  pinMode(TFT_BL_PIN, OUTPUT);
  digitalWrite(TFT_BL_PIN, HIGH);

  // Initialize TFT Display
  tft.init();
  tft.setRotation(currentRotation);

  // Disable TFT_eSPI internal multi-byte UTF-8 decoder so our single-byte
  // remapped character codes pass directly to the font renderer
  tft.setAttribute(UTF8_SWITCH, false);

  // Clear display and render initial greeting screen
  tft.fillScreen(themes[currentTheme].bg);
  redrawCurrentScreen();

  Serial.println("Ready!");
  Serial.println(" - Press Button 1 (IO 0) to cycle color themes.");
  Serial.println(" - Hold Button 1 (>1.5s) to rotate display 180 degrees.");
  Serial.println(" - Press Button 2 (IO 35) to switch screens.");
  Serial.println(" - Hold Button 2 (>1.5s) to toggle language (RU <-> VI).");
}

// ======================================================================================
// MAIN EVENT LOOP
// ======================================================================================
void loop() {
  unsigned long now = millis();

  // 1. Check Button states every 20ms with debouncing
  if (now - lastBtnCheck >= 20) {
    lastBtnCheck = now;

    bool btn1State = digitalRead(BTN1_PIN);
    bool btn2State = digitalRead(BTN2_PIN);

    // --- BUTTON 1 HANDLING (GPIO 0: Left / Top) ---
    if (btn1LastState == HIGH && btn1State == LOW) {
      btn1PressStart = now; // Pressed down
    } else if (btn1LastState == LOW && btn1State == HIGH) {
      // Released
      unsigned long duration = now - btn1PressStart;
      if (duration >= 1500) {
        // Long press (>1.5s): Rotate screen 180 degrees
        currentRotation = (currentRotation == 1) ? 3 : 1;
        tft.setRotation(currentRotation);
        tft.fillScreen(themes[currentTheme].bg);
        redrawCurrentScreen();
        Serial.printf("[Button 1 Long Press] Screen rotation flipped to: %d\n", currentRotation);
      } else if (duration >= 50) {
        // Short press: Cycle color theme
        currentTheme = (currentTheme + 1) % NUM_THEMES;
        tft.fillScreen(themes[currentTheme].bg);
        redrawCurrentScreen();
        Serial.printf("[Button 1] Switched theme to: %s\n", themes[currentTheme].name);
      }
    }
    btn1LastState = btn1State;

    // --- BUTTON 2 HANDLING (GPIO 35: Right / Bottom) ---
    if (btn2LastState == HIGH && btn2State == LOW) {
      btn2PressStart = now; // Pressed down
    } else if (btn2LastState == LOW && btn2State == HIGH) {
      // Released
      unsigned long duration = now - btn2PressStart;
      if (duration >= 1500) {
        // Long press (>1.5s): Toggle language RU <-> VI
        currentLang = (currentLang == LANG_RU) ? LANG_VI : LANG_RU;
        tft.fillScreen(themes[currentTheme].bg);
        redrawCurrentScreen();
        Serial.printf("[Button 2 Long Press] Language switched to: %s\n",
                      (currentLang == LANG_RU) ? "Russian (RU)" : "Vietnamese (VI)");
      } else if (duration >= 50) {
        // Short press: Switch to next screen
        currentScreen = (DisplayScreen)((currentScreen + 1) % SCREEN_COUNT);
        tft.fillScreen(themes[currentTheme].bg);
        redrawCurrentScreen();
        Serial.printf("[Button 2] Switched screen to index: %d\n", currentScreen);
      }
    }
    btn2LastState = btn2State;
  }

  // 2. Update uptime counter in footer once per second (flicker-free)
  if (now - lastUptimeUpdate >= 1000) {
    lastUptimeUpdate = now;
    drawFooter();
  }
}
