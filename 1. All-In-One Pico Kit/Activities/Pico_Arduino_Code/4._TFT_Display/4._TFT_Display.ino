/*
  =========================================================
   RP2040 + ST7789 Display + Touch (LovyanGFX)
  =========================================================

  LIBRARY USED:
  ---------------------------------------------------------
  - LovyanGFX (for display + touch control)

  CONNECTIONS:
  ---------------------------------------------------------
  SPI Display (ST7789):
    SCLK -> GPIO 6
    MOSI -> GPIO 7
    MISO -> Not Used
    DC   -> GPIO 16
    CS   -> GPIO 17
    RST  -> Not Used

  Touch Controller (FT5x06 - I2C):
    SDA -> GPIO 4
    SCL -> GPIO 5
    INT -> GPIO 25
    RST -> GPIO 24
    I2C Address -> 0x38

  Backlight:
    Control Pin -> GPIO 0

  DISPLAY:
  ---------------------------------------------------------
  Stage 1 -> "HELLO WORLD!"
  Stage 2 -> "Bye Bye"
  Stage 3 -> Clear screen
*/

#include <LovyanGFX.hpp> // Display + touch library

/*
  =========================================================
   CUSTOM DISPLAY CLASS (LGFX)
  =========================================================
  - Combines SPI bus, display panel, and touch controller
*/
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789 _panel_instance;   // Display driver (ST7789)
  lgfx::Bus_SPI _bus_instance;          // SPI communication bus
  lgfx::Touch_FT5x06 _touch_instance;   // Touch controller

public:
  LGFX(void)
  {
    // ==========================
    // SPI BUS CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();

    cfg.spi_host = 0;            // SPI host selection
    cfg.spi_mode = 0;            // SPI mode (Mode 0)
    cfg.freq_write = 40000000;   // Write speed (40 MHz)
    cfg.freq_read = 16000000;    // Read speed (16 MHz)

    cfg.pin_sclk = 6;            // Clock pin
    cfg.pin_mosi = 7;            // MOSI pin
    cfg.pin_miso = -1;           // Not used
    cfg.pin_dc   = 16;           // Data/Command pin

    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    // ==========================
    // DISPLAY PANEL CONFIGURATION
    // ==========================
    auto panel_cfg = _panel_instance.config();

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

    panel_cfg.readable = false;  // Display does not support read
    panel_cfg.invert   = true;   // Color inversion
    panel_cfg.rgb_order = false;

    panel_cfg.dlen_16bit = false;
    panel_cfg.bus_shared = false;

    _panel_instance.config(panel_cfg);

    // ==========================
    // TOUCH CONFIGURATION (I2C)
    // ==========================
    auto touch_cfg = _touch_instance.config();

    touch_cfg.x_min = 0;
    touch_cfg.x_max = 239;

    touch_cfg.y_min = 0;
    touch_cfg.y_max = 319;

    touch_cfg.pin_int = 25;      // Interrupt pin
    touch_cfg.pin_rst = 24;      // Reset pin

    touch_cfg.bus_shared = false;
    touch_cfg.offset_rotation = 0;

    touch_cfg.i2c_port = 0;
    touch_cfg.i2c_addr = 0x38;

    touch_cfg.pin_sda = 4;
    touch_cfg.pin_scl = 5;

    touch_cfg.freq = 400000;     // I2C speed (400 kHz)

    _touch_instance.config(touch_cfg);

    // Attach touch to display
    _panel_instance.setTouch(&_touch_instance);

    // Finalize panel setup
    setPanel(&_panel_instance);
  }
};

// ==========================
// CREATE DISPLAY OBJECT
// ==========================
LGFX gfx;

// ==========================
// SCREEN RESOLUTION
// ==========================
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

void setup() {

  // ==========================
  // SERIAL INITIALIZATION
  // ==========================
  Serial.begin(115200);

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  gfx.init();

  // Backlight control
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);     // Turn ON backlight
  gfx.setBrightness(255);    // Max brightness

  // ==========================
  // STAGE 1: HELLO WORLD
  // ==========================
  gfx.setTextSize(2);
  gfx.setTextColor(TFT_WHITE);
  gfx.setCursor(60, 100);
  gfx.print("HELLO WORLD!");

  delay(1000);

  // ==========================
  // STAGE 2: BYE BYE
  // ==========================
  gfx.fillScreen(TFT_BLACK);
  gfx.setCursor(100, 100);
  gfx.print("Bye Bye");

  delay(1000);

  // ==========================
  // STAGE 3: CLEAR SCREEN
  // ==========================
  gfx.fillScreen(TFT_BLACK);

  // digitalWrite(0, LOW); // Backlight OFF (optional)
}

void loop() {

  // ==========================
  // IDLE LOOP
  // ==========================
  delay(10);
}