/*
  =========================================================
   RP2040 + DHT20 + Display (Temperature & Humidity)
  =========================================================

  LIBRARIES USED:
  ---------------------------------------------------------
  - LovyanGFX (Display + Touch)
  - Wire (I2C Communication)
  - DHT20 (Temperature & Humidity Sensor)

  CONNECTIONS:
  ---------------------------------------------------------
  DHT20 Sensor (I2C):
    SDA -> GPIO 2
    SCL -> GPIO 3
    Address -> 0x38

  Display:
    SPI + Touch (Configured in LGFX class)

  Backlight:
    Control Pin -> GPIO 0

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Reads temperature and humidity every 1 second
  - Displays values centered on screen
  - Uses different colors for clarity
*/

#include <LovyanGFX.hpp>    // Display library
#include <Wire.h>           // I2C communication
#include "DHT20.h"          // DHT20 sensor library

/*
  =========================================================
   CUSTOM DISPLAY CLASS (LGFX)
  =========================================================
*/
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7789 _panel_instance; // Display driver
  lgfx::Bus_SPI _bus_instance;        // SPI bus
  lgfx::Touch_FT5x06 _touch_instance;// Touch controller

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

LGFX gfx;           // Create display object
DHT20 DHT(&Wire1);  // Create DHT20 sensor object using Wire1

unsigned long preMillis = 0; // Store last update time
const long interval = 1000;  // Update interval (1 second)

// ==========================
// SCREEN SIZE
// ==========================
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

void setup() {

  Serial.begin(115200); // Start serial communication

  // ==========================
  // DISPLAY SETUP
  // ==========================
  gfx.init();                 // Initialize display
  pinMode(0, OUTPUT);         // Backlight pin
  digitalWrite(0, HIGH);      // Turn ON backlight

  gfx.setTextColor(TFT_WHITE);// Text color
  gfx.setTextSize(3);         // Text size
  gfx.fillScreen(TFT_BLACK);  // Clear screen

  // ==========================
  // I2C SETUP FOR DHT20
  // ==========================
  Wire1.setSDA(2); // Set SDA pin
  Wire1.setSCL(3); // Set SCL pin
  Wire1.begin();   // Start I2C bus

  DHT.begin();     // Initialize DHT20 sensor
}

void loop() {

  unsigned long currentMillis = millis(); // Current time

  // ==========================
  // UPDATE EVERY 1 SECOND
  // ==========================
  if (currentMillis - preMillis >= interval) {

    // Check sensor read interval
    if (millis() - DHT.lastRead() >= 1000) {

      int status = DHT.read(); // Read sensor data

      if (status == DHT20_OK) { // If read successful

        gfx.fillScreen(TFT_BLACK); // Clear screen

        gfx.setTextSize(3); // Set text size

        // ==========================
        // READ SENSOR VALUES
        // ==========================
        float humidity = DHT.getHumidity();       // Get humidity
        float temperature = DHT.getTemperature(); // Get temperature

        int centerX = screenWidth / 2; // Center position

        // ==========================
        // DISPLAY HUMIDITY
        // ==========================
        gfx.setCursor(centerX - 6 * 3 * 5, 60); // Label position
        gfx.setTextColor(TFT_WHITE);
        gfx.print("Humidity:");

        gfx.setTextColor(TFT_BLUE); // Value color
        gfx.setCursor(centerX - 6 * 3 * 2, 100);
        gfx.printf("%.1f %%", humidity);

        // ==========================
        // DISPLAY TEMPERATURE
        // ==========================
        gfx.setTextColor(TFT_WHITE);
        gfx.setCursor(centerX - 6 * 3 * 6, 160);
        gfx.print("Temperature:");

        gfx.setTextColor(TFT_RED);
        gfx.setCursor(centerX - 6 * 3 * 2, 200);
        gfx.printf("%.1f C", temperature);
      }
    }

    preMillis = currentMillis; // Update last time
  }
}