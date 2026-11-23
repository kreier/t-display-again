# T-Display

Đủ loại ứng dụng cho chiếc T-Display của tôi.

[![Watch the video](https://img.youtube.com/vi/Rc5NstE1-d4/maxresdefault.jpg)](https://www.youtube.com/watch?v=Rc5NstE1-d4)

A collection of applications for the **LilyGO TTGO T-Display** (ESP32 with 4 MB Flash and ST7789 1.14" 240x135 IPS display).

---

## Hello World in Russian (Привет, мир!)

Version `0.1.0` introduces a dedicated firmware application that displays **"Привет, мир!"** ("Hello World!" in Russian) with full Cyrillic font support, interactive hardware button controls, multiple color themes, hardware diagnostics, and screen rotation.

```
+--------------------------------------------------+
| TTGO T-Display ESP32                   Cyberpunk |
|--------------------------------------------------|
|  *                                            *  |
|                                                  |
|                  Привет, мир!                    |
|                  Hello World!                    |
|                                                  |
|       ESP32-D0WDQ6 * 4MB FLASH * ST7789          |
|--------------------------------------------------|
| [L]Theme  [R]Info                          00:42 |
+--------------------------------------------------+
```

### Features

- **Full Russian Cyrillic Support**: UTF-8 decoding rendered with embedded anti-aliased FreeSans fonts (`FreeSansBold12pt8b` and `FreeSans9pt8b`), completely self-contained with no SPIFFS/LittleFS flashing required.
- **5 Selectable Color Themes**:
  1. **Cyberpunk**: Neon cyan and magenta accents on midnight dark.
  2. **Emerald**: Classic Matrix green on charcoal.
  3. **Sunset**: Vibrant amber gold and coral on dark navy.
  4. **Nordic Ice**: Glacier white and cyan on slate blue.
  5. **Amber CRT**: Retro phosphor amber on pure black.
- **3 Interactive Screens**:
  - **Greeting Card**: "Привет, мир!" headline, "Hello World!" subtitle, and board badge.
  - **System Diagnostics**: Live ESP32 CPU clock frequency (240 MHz), Flash size (4 MB), free heap memory, and SDK version.
  - **Russian Alphabet Test**: Complete Cyrillic uppercase and lowercase font showcase (`А-Я`, `а-я`, `Ё`, `ё`).
- **180° Display Rotation**: Flip orientation on the fly so the USB port can be on the left or right.
- **Live Footer**: Real-time MM:SS uptime counter refreshed without screen flickering.
- **Diagnostic Logging**: Serial monitor output at 115200 baud with chip specs and button event logs.

---

## Hardware Specifications & Pinout

The original **LilyGO TTGO T-Display** is powered by an Espressif ESP32-D0WDQ6 chip and features an onboard 1.14-inch IPS LCD.

| Component | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **TFT MOSI** | `GPIO 19` | SPI Data Out |
| **TFT SCLK** | `GPIO 18` | SPI Clock |
| **TFT CS** | `GPIO 5` | SPI Chip Select |
| **TFT DC** | `GPIO 16` | Data / Command Control |
| **TFT RST** | `GPIO 23` | Display Reset |
| **TFT Backlight** | `GPIO 4` | Backlight power (Active HIGH) |
| **Button 1 (Left / Top)** | `GPIO 0` | Boot button (Active LOW, internal pull-up) |
| **Button 2 (Right / Bottom)** | `GPIO 35` | Auxiliary button (Active LOW, input-only) |
| **Battery ADC** | `GPIO 34` | Voltage divider for battery monitoring |

---

## Controls

| Action | Control | Function |
| :--- | :--- | :--- |
| **Short Press Button 1** (GPIO 0) | Left / Top Button | Cycle through color themes |
| **Long Press Button 1** (>1.5s) | Left / Top Button | Rotate display 180° (USB left / right) |
| **Short Press Button 2** (GPIO 35) | Right / Bottom Button | Switch screen (Greeting -> System Info -> Alphabet) |

---

## How to Build & Flash

### Method 1: PlatformIO (Recommended)

This repository includes a ready-to-use `platformio.ini` configured with all hardware definitions and the `TFT_eSPI` library.

1. Connect your T-Display via USB-C.
2. Build and upload firmware:
   ```bash
   pio run -t upload
   ```
3. Open Serial Monitor (115200 baud):
   ```bash
   pio device monitor
   ```

### Method 2: Arduino IDE

1. Open Arduino IDE and install the **ESP32** board package (`esp32` by Espressif).
2. Install the **TFT_eSPI** library via the Library Manager.
3. Configure `TFT_eSPI` for the TTGO T-Display:
   - Navigate to your Arduino libraries folder: `libraries/TFT_eSPI/User_Setup_Select.h`
   - Comment out `#include <User_Setup.h>`
   - Uncomment `#include <User_Setups/Setup25_TTGO_T_Display.h>`
4. In Arduino IDE:
   - Board: **ESP32 Dev Module**
   - Flash Size: **4MB (32Mb)**
   - Upload Speed: **921600** (or 115200)
5. Open [`firmware/hello_world.ino`](firmware/hello_world.ino) and click **Upload**.

---

## How Cyrillic / Russian Text Works

Standard Arduino graphics fonts only contain ASCII codes (0x20 to 0x7E) and do not include Cyrillic glyphs. Modern C++ compilers encode string literals in **UTF-8**, where Cyrillic letters require 2 bytes:

- Prefix `0xD0` followed by `0x90..0xBF` (`А`–`Я`, `а`–`п`) or `0x81` (`Ё`)
- Prefix `0xD1` followed by `0x80..0x8F` (`р`–`я`) or `0x91` (`ё`)

This project bundles custom Cyrillic GFX fonts (`FreeSansBold12.h` and `FreeSans9.h`) mapped to single-byte indices:

```cpp
String utf8rus(const char* source) {
  String target = "";
  int i = 0, k = strlen(source);
  while (i < k) {
    unsigned char n = source[i++];
    if (n >= 127) {
      switch (n) {
        case 208: { // 0xD0
          n = source[i++];
          if (n == 129) n = 192; // 'Ё'
          break;
        }
        case 209: { // 0xD1
          n = source[i++];
          if (n == 145) n = 193; // 'ё'
          break;
        }
      }
    }
    target += (char)n;
  }
  return target;
}
```

By disabling the library's internal multi-byte decoder (`tft.setAttribute(UTF8_SWITCH, false);`), strings like `utf8rus("Привет, мир!")` render directly to the screen without requiring external font files on flash partitions.

---

## Project Structure

```
├── .gitignore               # Git ignore rules (build files, .pio, etc.)
├── LICENSE                  # MIT License
├── README.md                # Project documentation
├── platformio.ini           # PlatformIO project configuration
└── firmware/
    ├── hello_world.ino      # Main application source code
    ├── FreeSansBold12.h     # 12pt Bold Cyrillic GFX font
    └── FreeSans9.h          # 9pt Cyrillic GFX font
```

---

## License

This project is licensed under the [MIT License](LICENSE).
