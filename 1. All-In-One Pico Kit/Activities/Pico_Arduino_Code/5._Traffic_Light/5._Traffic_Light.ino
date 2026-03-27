/*
  =========================================================
   RP2040 Traffic Light System with Display (LovyanGFX)
  =========================================================

  LIBRARY USED:
  ---------------------------------------------------------
  - LovyanGFX (Display + Touch Control)

  CONNECTIONS:
  ---------------------------------------------------------
  LEDs:
    Red LED    -> GPIO 18
    Yellow LED -> GPIO 20
    Green LED  -> GPIO 19

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
  - Simulates a traffic light system
  - Displays countdown timer on screen
  - Automatic stage switching:
      GREEN → YELLOW → RED → GREEN
  - Last 5 seconds → LED blinking effect
*/

#include <LovyanGFX.hpp>

// ==========================
// LED PIN DEFINITIONS
// ==========================
#define Red_LED 18
#define Yellow_LED 20
#define Green_LED 19

/*
  =========================================================
   CUSTOM DISPLAY CLASS (LGFX)
  =========================================================
*/
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void)
  {
    // ==========================
    // SPI CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();

    cfg.spi_host = 0;
    cfg.spi_mode = 0;
    cfg.freq_write = 80000000;  // 80 MHz write speed
    cfg.freq_read  = 16000000;  // 16 MHz read speed

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
// SCREEN SIZE
// ==========================
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

// ==========================
// TRAFFIC LIGHT STATES
// ==========================
enum Stage { GREEN_STAGE, YELLOW_STAGE, RED_STAGE };

// ==========================
// GLOBAL VARIABLES
// ==========================
Stage currentStage = GREEN_STAGE; // Current traffic light stage
uint32_t remainingTime = 30;      // Countdown timer
uint32_t lastUpdate = 0;          // Last time countdown updated
uint32_t lastBlink = 0;           // Last blink time
bool blinkState = false;          // Blink ON/OFF state

// ==========================
// TEXT DISPLAY AREA
// ==========================
const int TEXT_AREA_X = 210;
const int TEXT_AREA_Y = 100;
const int TEXT_AREA_WIDTH  = 60;
const int TEXT_AREA_HEIGHT = 30;

void setup() {

  // ==========================
  // SERIAL INIT
  // ==========================
  Serial.begin(115200);
    
  // ==========================
  // DISPLAY INIT
  // ==========================
  gfx.init();
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH); // Backlight ON
    
  // ==========================
  // LED INIT
  // ==========================
  pinMode(Red_LED, OUTPUT);
  pinMode(Yellow_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);
    
  // ==========================
  // INITIAL STATE (GREEN)
  // ==========================
  digitalWrite(Green_LED, HIGH);

  // ==========================
  // DISPLAY STATIC TEXT
  // ==========================
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_WHITE);

  gfx.setCursor(30, 100);
  gfx.print("Countdown:");

  // Initial countdown value
  gfx.setCursor(TEXT_AREA_X, TEXT_AREA_Y);
  gfx.printf("%2ds", remainingTime);

  lastUpdate = millis();
}

void loop() {

  uint32_t now = millis();

  // ==========================
  // COUNTDOWN UPDATE (1 SEC)
  // ==========================
  if (now - lastUpdate >= 1000) {

    remainingTime--;
    lastUpdate = now;

    // Clear only number area
    gfx.fillRect(TEXT_AREA_X, TEXT_AREA_Y, TEXT_AREA_WIDTH, TEXT_AREA_HEIGHT, TFT_BLACK);
      
    // Update number
    gfx.setTextColor(TFT_WHITE);
    gfx.setCursor(TEXT_AREA_X, TEXT_AREA_Y);
    gfx.printf("%2ds", remainingTime);

    // ==========================
    // STAGE SWITCHING
    // ==========================
    if (remainingTime == 0) {

      // Turn OFF all LEDs
      digitalWrite(Green_LED, LOW);
      digitalWrite(Yellow_LED, LOW);
      digitalWrite(Red_LED, LOW);

      switch(currentStage) {

        case GREEN_STAGE:
          currentStage = YELLOW_STAGE;
          remainingTime = 3;
          digitalWrite(Yellow_LED, HIGH);
          break;
            
        case YELLOW_STAGE:
          currentStage = RED_STAGE;
          remainingTime = 20;
          digitalWrite(Red_LED, HIGH);
          break;
            
        case RED_STAGE:
          currentStage = GREEN_STAGE;
          remainingTime = 30;
          digitalWrite(Green_LED, HIGH);
          break;
      }
    }
  }

  // ==========================
  // BLINK LOGIC (LAST 5 SEC)
  // ==========================
  if (remainingTime > 0 && remainingTime <= 5) {

    if (now - lastBlink >= 500) {

      blinkState = !blinkState;
      lastBlink = now;
        
      switch(currentStage) {
        case GREEN_STAGE:
          digitalWrite(Green_LED, blinkState);
          break;
        case RED_STAGE:
          digitalWrite(Red_LED, blinkState);
          break;
      }
    }

  } else {

    // ==========================
    // NORMAL LED STATE
    // ==========================
    switch(currentStage) {
      case GREEN_STAGE:
        digitalWrite(Green_LED, HIGH);
        break;
      case YELLOW_STAGE:
        digitalWrite(Yellow_LED, HIGH);
        break;
      case RED_STAGE:
        digitalWrite(Red_LED, HIGH);
        break;
    }
  }

  delay(10);
}