/*
  =========================================================
   RP2040 + Ultrasonic Sensor + Display (LovyanGFX)
  =========================================================

  LIBRARIES USED:
  ---------------------------------------------------------
  - HCSR04 (Ultrasonic Distance Sensor)
  - LovyanGFX (Display + Touch Control)

  CONNECTIONS:
  ---------------------------------------------------------
  Ultrasonic Sensor (HC-SR04):
    VCC  -> 5V
    GND  -> GND
    TRIG -> GPIO 9
    ECHO -> GPIO 8

  Display (ST7789 - SPI):
    SCLK -> GPIO 6
    MOSI -> GPIO 7
    DC   -> GPIO 16
    CS   -> GPIO 17

  Touch (FT5x06 - I2C):
    SDA -> GPIO 4
    SCL -> GPIO 5
    INT -> GPIO 25
    RST -> GPIO 24
    Address -> 0x38

  Backlight:
    Control Pin -> GPIO 0

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Measures distance using ultrasonic sensor
  - Applies averaging for stable readings
  - Displays distance on screen
  - Updates every 500 ms
*/

#include <HCSR04.h>
#include <LovyanGFX.hpp>

// ==========================
// ULTRASONIC SENSOR PINS
// ==========================
const byte triggerPin = 9;
const byte echoPin    = 8;

// Create sensor object
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

/*
  =========================================================
   DISPLAY DRIVER CLASS (LGFX)
  =========================================================
*/
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void) {

    // ==========================
    // SPI CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();

    cfg.spi_host = 0;
    cfg.spi_mode = 0;
    cfg.freq_write = 80000000;
    cfg.freq_read  = 16000000;

    cfg.pin_sclk = 6;
    cfg.pin_mosi = 7;
    cfg.pin_miso = -1;
    cfg.pin_dc   = 16;

    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    // ==========================
    // DISPLAY PANEL CONFIG
    // ==========================
    auto panel_cfg = _panel_instance.config();

    panel_cfg.pin_cs   = 17;
    panel_cfg.pin_rst  = -1;
    panel_cfg.pin_busy = -1;

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

    _panel_instance.config(panel_cfg);

    // ==========================
    // TOUCH CONFIGURATION
    // ==========================
    auto touch_cfg = _touch_instance.config();

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

    _touch_instance.config(touch_cfg);
    _panel_instance.setTouch(&_touch_instance);

    setPanel(&_panel_instance);
  }
};

// ==========================
// DISPLAY OBJECT
// ==========================
LGFX gfx;

// ==========================
// UPDATE INTERVAL
// ==========================
const unsigned long UPDATE_INTERVAL = 500;
unsigned long lastUpdateTime = 0;

// ==========================
// TEXT DISPLAY AREA
// ==========================
const int TEXT_AREA_X = 160;
const int TEXT_AREA_Y = 100;
const int TEXT_AREA_WIDTH  = 60;
const int TEXT_AREA_HEIGHT = 30;

/*
  =========================================================
   FUNCTION: getStableDistance
  =========================================================
  - Takes multiple readings (3 samples)
  - Filters invalid values
  - Returns average distance
*/
float getStableDistance() {

  float total = 0;
  int validCount = 0;

  for (int i = 0; i < 3; i++) {

    float d = distanceSensor.measureDistanceCm();

    if (d > 0 && d < 500) {
      total += d;
      validCount++;
    }

    delay(20);
  }

  if (validCount == 0) return -1;

  return total / validCount;
}

void setup() {

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  gfx.init();

  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH); // Backlight ON

  gfx.setRotation(0);
  gfx.fillScreen(TFT_BLACK);

  // ==========================
  // TEXT SETUP
  // ==========================
  gfx.setTextSize(2);
  gfx.setTextColor(TFT_WHITE);

  gfx.setCursor(30, 100);
  gfx.print("Distance:");

  // ==========================
  // INITIAL DISTANCE DISPLAY
  // ==========================
  float initialDistance = getStableDistance();

  gfx.setCursor(TEXT_AREA_X, TEXT_AREA_Y);

  if (initialDistance > 0) {
    gfx.printf("%.2f cm", initialDistance);
  } else {
    gfx.print("Out of range");
  }
}

void loop() {

  unsigned long currentTime = millis();

  // ==========================
  // PERIODIC UPDATE
  // ==========================
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {

    lastUpdateTime = currentTime;

    float distance = getStableDistance();

    // Clear previous value
    gfx.fillRect(TEXT_AREA_X, TEXT_AREA_Y, TEXT_AREA_WIDTH + 50, TEXT_AREA_HEIGHT, TFT_BLACK);

    gfx.setTextColor(TFT_WHITE);
    gfx.setCursor(TEXT_AREA_X, TEXT_AREA_Y);

    // ==========================
    // DISPLAY NEW VALUE
    // ==========================
    if (distance > 0) {
      gfx.printf("%.2f cm", distance);
    } else {
      gfx.print("Out of range");
    }
  }

  // ==========================
  // SMALL DELAY
  // ==========================
  delay(10);
}