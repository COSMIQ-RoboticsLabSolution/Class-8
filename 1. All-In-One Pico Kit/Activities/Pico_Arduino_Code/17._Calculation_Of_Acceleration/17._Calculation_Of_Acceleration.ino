/*
  =========================================================
   Arduino + TFT Display + LSM6DS3TRC Accelerometer
   Libraries Used:
     - LovyanGFX.hpp
     - Adafruit_LSM6DS3TRC.h
     - Wire.h
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Reads X, Y, Z acceleration values
  - Displays real-time data on TFT screen
  - Updates values periodically
  - Clears previous values to avoid display ghosting
*/

#include <LovyanGFX.hpp>            // TFT display control library
#include <Adafruit_LSM6DS3TRC.h>    // Accelerometer sensor library
#include <Wire.h>                   // I2C communication library

// ==========================
// CUSTOM LGFX DISPLAY CLASS
// ==========================
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7789 _panel_instance;   // ST7789 display driver
  lgfx::Bus_SPI _bus_instance;          // SPI communication bus
  lgfx::Touch_FT5x06 _touch_instance;   // Touch controller

public:
  LGFX(void) {

    // ==========================
    // SPI CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();  
    cfg.spi_host = 0;                   
    cfg.spi_mode = 0;                   
    cfg.freq_write = 80000000;          // Write speed (80 MHz)
    cfg.freq_read = 16000000;           // Read speed (16 MHz)
    cfg.pin_sclk = 6;                   
    cfg.pin_mosi = 7;                   
    cfg.pin_miso = -1;                  // Not used
    cfg.pin_dc = 16;                    
    _bus_instance.config(cfg);          
    _panel_instance.setBus(&_bus_instance);

    // ==========================
    // DISPLAY PANEL CONFIG
    // ==========================
    auto panel_cfg = _panel_instance.config();
    panel_cfg.pin_cs = 17;              
    panel_cfg.pin_rst = -1;             
    panel_cfg.pin_busy = -1;            
    panel_cfg.memory_width = 240;       
    panel_cfg.memory_height = 320;      
    panel_cfg.panel_width = 240;        
    panel_cfg.panel_height = 320;       
    panel_cfg.offset_x = 0;             
    panel_cfg.offset_y = 0;             
    panel_cfg.offset_rotation = 1;      
    panel_cfg.dummy_read_pixel = 8;     
    panel_cfg.dummy_read_bits = 1;      
    panel_cfg.readable = false;         
    panel_cfg.invert = true;            
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

    setPanel(&_panel_instance);         // Finalize display setup
  }
};

LGFX gfx;                               // Display object
Adafruit_LSM6DS3TRC lsm6ds3trc;         // Accelerometer object

// ==========================
// DISPLAY PARAMETERS
// ==========================
const uint16_t SCREEN_WIDTH = 240;      // Screen width
const uint16_t SCREEN_HEIGHT = 320;     // Screen height
const uint16_t UPDATE_INTERVAL = 150;   // Refresh interval (ms)
const uint8_t TEXT_SIZE = 2;            // Text size
const uint16_t TEXT_COLOR = TFT_WHITE;  // Text color
const uint16_t BG_COLOR = TFT_BLACK;    // Background color

// ==========================
// AXIS DISPLAY LAYOUT
// ==========================
const struct {
  uint16_t x_label;   // Label X position
  uint16_t y_label;   // Label Y position
  uint16_t x_value;   // Value X position
  uint16_t y_value;   // Value Y position
  uint16_t w;         // Width of value area
  uint16_t h;         // Height of value area
} AXIS_AREA[3] = {

  {20, 60, 80, 60, 150, 24},   // X-axis display area
  {20, 120, 80, 120, 150, 24}, // Y-axis display area
  {20, 180, 80, 180, 150, 24}  // Z-axis display area
};

void setup() {

  Serial.begin(115200);                  // Start serial communication

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  gfx.init();                            // Initialize display
  pinMode(0, OUTPUT);                    // Backlight control pin
  digitalWrite(0, HIGH);                 // Turn ON backlight
  gfx.fillScreen(BG_COLOR);              // Clear screen

  // ==========================
  // SENSOR INITIALIZATION
  // ==========================
  Wire1.setSDA(2);                       
  Wire1.setSCL(3);                       
  Wire1.begin();                         
  delay(100);                            

  if (!lsm6ds3trc.begin_I2C(0x6B, &Wire1)) { // Initialize sensor
    gfx.setTextColor(TFT_RED);           
    gfx.setTextSize(TEXT_SIZE);          
    gfx.setCursor(20, SCREEN_HEIGHT / 2 - 20);
    gfx.print("Sensor initialization failed!"); // Error message
    while (1) delay(10);                 // Stop execution
  }

  // ==========================
  // STATIC UI TEXT
  // ==========================
  gfx.setTextSize(TEXT_SIZE);            
  gfx.setTextColor(TEXT_COLOR);          

  gfx.setCursor(20, 20);
  gfx.print("Accelerometer data");       // Title

  gfx.setCursor(AXIS_AREA[0].x_label, AXIS_AREA[0].y_label);
  gfx.print("X:");                       // X label

  gfx.setCursor(AXIS_AREA[1].x_label, AXIS_AREA[1].y_label);
  gfx.print("Y:");                       // Y label

  gfx.setCursor(AXIS_AREA[2].x_label, AXIS_AREA[2].y_label);
  gfx.print("Z:");                       // Z label
}

void loop() {

  static unsigned long lastUpdate = 0;   // Store last update time

  if (millis() - lastUpdate < UPDATE_INTERVAL) return; // Wait for interval

  lastUpdate = millis();                 // Update timer

  // ==========================
  // READ SENSOR DATA
  // ==========================
  sensors_event_t accel;                 
  lsm6ds3trc.getEvent(&accel, nullptr, nullptr); // Read accelerometer

  // ==========================
  // DISPLAY EACH AXIS
  // ==========================
  for (int i = 0; i < 3; i++) {

    float value = 0;                     // Store axis value

    switch (i) {
      case 0: value = accel.acceleration.x; break; // X-axis
      case 1: value = accel.acceleration.y; break; // Y-axis
      case 2: value = accel.acceleration.z; break; // Z-axis
    }

    // --------------------------
    // CLEAR OLD VALUE (ANTI-GHOSTING)
    // --------------------------
    gfx.fillRect(
      AXIS_AREA[i].x_value,
      AXIS_AREA[i].y_value,
      AXIS_AREA[i].w,
      AXIS_AREA[i].h,
      BG_COLOR
    );

    // --------------------------
    // PRINT NEW VALUE
    // --------------------------
    gfx.setTextColor(TEXT_COLOR);        
    gfx.setCursor(AXIS_AREA[i].x_value, AXIS_AREA[i].y_value);
    gfx.printf("%.2f m/s^2", value);     // Show value with 2 decimals
  }
}