/*
  =========================================================
   Arduino + TFT Display + Hall Sensor Counter System
   Library Used:
     - LovyanGFX.hpp
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Detects magnetic field using Hall sensor
  - Increments counter when magnet is detected
  - Displays count on TFT screen
  - LED blinks on each detection

  APPLICATION:
  ---------------------------------------------------------
  Object Counter / Rotation Counter / Magnetic Detection
*/

#include <LovyanGFX.hpp>          // TFT display control library

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

// ==========================
// PIN DEFINITIONS
// ==========================
const int hall = 21;        // Hall sensor input pin
const int redLed = 18;      // Red LED output pin

int count = 0;              // Counter variable

// ==========================
// DISPLAY FUNCTION
// ==========================
void updateCounterDisplay(int count) {

  gfx.fillScreen(TFT_BLACK);        // Clear screen

  gfx.setCursor(60, 100, 4);        // Set cursor position (with font)
  gfx.setTextSize(1);               // Set text scaling
  gfx.setTextColor(TFT_WHITE);      // Set text color

  gfx.printf("counter: %d", count); // Display counter value
}

void setup() {

  Serial.begin(115200);             // Start serial communication

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  gfx.init();                       // Initialize display
  pinMode(0, OUTPUT);               // Backlight pin
  digitalWrite(0, HIGH);            // Turn ON backlight

  // ==========================
  // PIN MODES
  // ==========================
  pinMode(hall, INPUT);             // Hall sensor input
  pinMode(redLed, OUTPUT);          // LED output

  updateCounterDisplay(count);      // Show initial counter value
}

void loop() {

  delay(10);                        // Small delay for stability

  // ==========================
  // HALL SENSOR DETECTION
  // ==========================
  if (digitalRead(hall) == LOW) {   // Magnet detected (LOW signal)

    delay(50);                      // Debounce delay

    if (digitalRead(hall) == LOW) { // Confirm stable detection

      // --------------------------
      // WAIT FOR MAGNET TO LEAVE
      // --------------------------
      while (digitalRead(hall) == LOW); // Prevent multiple counts

      // --------------------------
      // UPDATE COUNTER
      // --------------------------
      count++;                      // Increment count
      updateCounterDisplay(count); // Update display

      // --------------------------
      // LED FEEDBACK
      // --------------------------
      digitalWrite(redLed, HIGH);   // LED ON
      delay(200);                  // Keep ON briefly
      digitalWrite(redLed, LOW);    // LED OFF
    }
  }
}