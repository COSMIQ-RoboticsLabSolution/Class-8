/*
  =========================================================
   RP2040 + Slider + LEDs + Display (Brightness Control)
  =========================================================

  LIBRARIES USED:
  ---------------------------------------------------------
  - LovyanGFX (Display + Touch)
  - Arduino Core

  CONNECTIONS:
  ---------------------------------------------------------
  Slider (Potentiometer):
    OUT -> GPIO 28

  LEDs:
    Red    -> GPIO 18
    Yellow -> GPIO 20
    Green  -> GPIO 19

  Display:
    SPI + Touch (Configured in LGFX class)

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Slider controls LED brightness (PWM)
  - Brightness level (0–10) displayed on screen
  - Updates only when value changes
*/

#include <LovyanGFX.hpp> // Display library
#include <Arduino.h>     // Arduino core

/*
  =========================================================
   CUSTOM DISPLAY CLASS (LGFX)
  =========================================================
*/
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7789 _panel_instance; // Display driver
  lgfx::Bus_SPI _bus_instance;        // SPI bus
  lgfx::Touch_FT5x06 _touch_instance; // Touch controller

public:
  LGFX(void) {

    auto cfg = _bus_instance.config(); // Get SPI config

    cfg.spi_host = 0;            // SPI host
    cfg.spi_mode = 0;            // SPI mode
    cfg.freq_write = 80000000;   // Write speed
    cfg.freq_read  = 16000000;   // Read speed

    cfg.pin_sclk = 6;            // Clock pin
    cfg.pin_mosi = 7;            // MOSI pin
    cfg.pin_miso = -1;           // Not used
    cfg.pin_dc   = 16;           // Data/Command pin

    _bus_instance.config(cfg);   // Apply config
    _panel_instance.setBus(&_bus_instance); // Attach bus

    auto panel_cfg = _panel_instance.config(); // Panel config

    panel_cfg.pin_cs   = 17;     // Chip Select
    panel_cfg.pin_rst  = -1;     // Reset not used
    panel_cfg.pin_busy = -1;     // Busy not used

    panel_cfg.memory_width  = 240;
    panel_cfg.memory_height = 320;

    panel_cfg.panel_width  = 240;
    panel_cfg.panel_height = 320;

    panel_cfg.offset_x = 0;
    panel_cfg.offset_y = 0;
    panel_cfg.offset_rotation = 1;

    panel_cfg.dummy_read_pixel = 8;
    panel_cfg.dummy_read_bits  = 1;

    panel_cfg.readable = false;
    panel_cfg.invert   = true;
    panel_cfg.rgb_order = false;

    panel_cfg.dlen_16bit = false;
    panel_cfg.bus_shared = false;

    _panel_instance.config(panel_cfg); // Apply panel config

    auto touch_cfg = _touch_instance.config(); // Touch config

    touch_cfg.x_min = 0;
    touch_cfg.x_max = 239;

    touch_cfg.y_min = 0;
    touch_cfg.y_max = 319;

    touch_cfg.pin_int = 25;
    touch_cfg.pin_rst = 24;

    touch_cfg.bus_shared = false;
    touch_cfg.offset_rotation = 0;

    touch_cfg.i2c_port = 0;
    touch_cfg.i2c_addr = 0x38;

    touch_cfg.pin_sda = 4;
    touch_cfg.pin_scl = 5;

    touch_cfg.freq = 400000;

    _touch_instance.config(touch_cfg); // Apply touch config
    _panel_instance.setTouch(&_touch_instance); // Attach touch

    setPanel(&_panel_instance); // Finalize display
  }
};

LGFX gfx; // Create display object

// ==========================
// PIN DEFINITIONS
// ==========================
#define SLIDER_PIN     28  // Analog slider pin
#define RED_LED_PIN    18  // Red LED
#define YELLOW_LED_PIN 20  // Yellow LED
#define GREEN_LED_PIN  19  // Green LED

int lastLevel = -1;  // Store previous brightness level

void setup() {

  Serial.begin(115200); // Start serial communication

  // ==========================
  // LED SETUP
  // ==========================
  pinMode(RED_LED_PIN, OUTPUT);    // Red LED output
  pinMode(YELLOW_LED_PIN, OUTPUT); // Yellow LED output
  pinMode(GREEN_LED_PIN, OUTPUT);  // Green LED output

  // ==========================
  // DISPLAY SETUP
  // ==========================
  gfx.init();                 // Initialize display
  pinMode(0, OUTPUT);         // Backlight control pin
  digitalWrite(0, HIGH);      // Turn ON backlight

  gfx.fillScreen(TFT_BLACK);  // Clear screen
  gfx.setTextSize(3);         // Text size
  gfx.setTextColor(TFT_WHITE);// Text color

  gfx.setCursor(30, 100);     // Text position
  gfx.print("Brightness:");   // Static label
}

void loop() {

  int analogValue = analogRead(SLIDER_PIN); // Read slider value

  int pwmValue = map(analogValue, 0, 900, 0, 255); // Convert to PWM
  pwmValue = constrain(pwmValue, 0, 255);          // Limit range

  int level = map(analogValue, 0, 900, 0, 10); // Convert to level (0–10)
  level = constrain(level, 0, 10);             // Limit range

  // ==========================
  // LED BRIGHTNESS CONTROL
  // ==========================
  analogWrite(RED_LED_PIN, pwmValue);    // Set Red LED brightness
  analogWrite(YELLOW_LED_PIN, pwmValue); // Set Yellow LED brightness
  analogWrite(GREEN_LED_PIN, pwmValue);  // Set Green LED brightness

  // ==========================
  // UPDATE DISPLAY ONLY IF CHANGED
  // ==========================
  if (level != lastLevel) {

    lastLevel = level; // Save new level

    // Clear previous number
    gfx.fillRect(240, 100, 60, 30, TFT_BLACK);

    gfx.setCursor(240, 100);    // Position for level
    gfx.setTextColor(TFT_GREEN);// Green text
    gfx.print(level);           // Display level

    // ==========================
    // SERIAL DEBUG
    // ==========================
    Serial.print("ADC: ");
    Serial.print(analogValue);

    Serial.print(" | PWM: ");
    Serial.print(pwmValue);

    Serial.print(" | Level: ");
    Serial.println(level);
  }

  delay(100); // Small delay
}