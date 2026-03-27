/*
  =========================================================
   Arduino + TFT Display + Touch Sensor + Relay Control
   Library Used:
     - LovyanGFX.hpp
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Displays message on TFT screen
  - Detects touch input
  - Activates relay (door open simulation)
  - Shows welcome message
  - Closes relay after delay

  APPLICATION:
  ---------------------------------------------------------
  Smart Door / Touch-Based Access System
*/

#include <LovyanGFX.hpp>          // TFT display control library

// ==========================
// CUSTOM LGFX DISPLAY CLASS
// ==========================
class LGFX : public lgfx::LGFX_Device {   // Inherit from display device

  lgfx::Panel_ST7789 _panel_instance;     // ST7789 display driver
  lgfx::Bus_SPI _bus_instance;            // SPI communication bus
  lgfx::Touch_FT5x06 _touch_instance;     // Touch controller

public:
  LGFX(void) {

    // ==========================
    // SPI CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();    // Get default SPI config
    cfg.spi_host = 0;                     
    cfg.spi_mode = 0;                     
    cfg.freq_write = 80000000;            // Write frequency (80 MHz)
    cfg.freq_read = 16000000;             // Read frequency (16 MHz)
    cfg.pin_sclk = 6;                     
    cfg.pin_mosi = 7;                     
    cfg.pin_miso = -1;                    // Not used
    cfg.pin_dc = 16;                      
    _bus_instance.config(cfg);            
    _panel_instance.setBus(&_bus_instance); // Attach SPI to panel

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
    _panel_instance.setTouch(&_touch_instance); // Attach touch to display

    setPanel(&_panel_instance);           // Finalize display setup
  }
};

LGFX gfx;                                // Display object

// ==========================
// PIN DEFINITIONS
// ==========================
const int relayPin = 12;       // Relay control pin (door mechanism)
const int touchPin = 14;       // Touch sensor input pin
const int backlightPin = 0;    // TFT backlight control pin

void setup() {

  Serial.begin(115200);                     // Start serial communication

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  gfx.init();                               // Initialize display
  pinMode(backlightPin, OUTPUT);            // Set backlight pin as output
  digitalWrite(backlightPin, HIGH);         // Turn ON backlight

  // ==========================
  // PIN INITIALIZATION
  // ==========================
  pinMode(relayPin, OUTPUT);                // Set relay pin as output
  pinMode(touchPin, INPUT);                 // Set touch sensor as input
  digitalWrite(relayPin, LOW);              // Ensure relay is OFF initially

  // ==========================
  // INITIAL SCREEN MESSAGE
  // ==========================
  gfx.fillScreen(TFT_BLACK);                // Clear screen
  gfx.setTextColor(TFT_WHITE);              // Set text color
  gfx.setTextSize(2);                       // Set text size
  gfx.setCursor(20, 100);                   // Set cursor position
  gfx.print("Please open the door");        // Display message
}

void loop() {

  // ==========================
  // TOUCH DETECTION
  // ==========================
  if (digitalRead(touchPin)) {              // If touch sensor is triggered

    // --------------------------
    // DOOR OPEN (RELAY ON)
    // --------------------------
    digitalWrite(relayPin, HIGH);           // Activate relay (open door)

    gfx.fillScreen(TFT_BLACK);              // Clear screen
    gfx.setCursor(60, 100);                 // Set cursor position
    gfx.print("Welcome");                   // Display welcome message

    delay(10000);                           // Keep door open for 10 seconds

    // --------------------------
    // DOOR CLOSE (RELAY OFF)
    // --------------------------
    digitalWrite(relayPin, LOW);            // Deactivate relay (close door)

    gfx.fillScreen(TFT_BLACK);              // Clear screen
    gfx.setCursor(20, 100);                 // Set cursor position
    gfx.print("Please open the door");      // Reset message
  }

  delay(100);                               // Small delay for stability
}