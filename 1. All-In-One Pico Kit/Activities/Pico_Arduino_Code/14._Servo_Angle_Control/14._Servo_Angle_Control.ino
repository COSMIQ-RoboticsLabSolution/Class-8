/*
  =========================================================
   Arduino + IR Remote + Servo Control + TFT Display
   Libraries Used:
     - Servo.h
     - IRremote.h
     - LovyanGFX.hpp
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Enter angle using IR remote (0–9 buttons)
  - Display input on TFT screen
  - Rotate servo to entered angle
  - Enable/Disable servo using remote
*/

#include <Servo.h>              // Servo motor control library
#include <IRremote.h>           // IR remote handling library
#include <LovyanGFX.hpp>        // TFT display control library

// ==========================
// CUSTOM LGFX DISPLAY CLASS
// ==========================
class LGFX : public lgfx::LGFX_Device {     // Inherit display device class

  lgfx::Panel_ST7789 _panel_instance;       // ST7789 display driver
  lgfx::Bus_SPI _bus_instance;              // SPI communication bus
  lgfx::Touch_FT5x06 _touch_instance;       // Touch controller

public:
  LGFX(void) {

    // ==========================
    // SPI CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();      // Get default SPI config
    cfg.spi_host = 0;                       
    cfg.spi_mode = 0;                       
    cfg.freq_write = 80000000;              // Write speed (80 MHz)
    cfg.freq_read = 16000000;               // Read speed (16 MHz)
    cfg.pin_sclk = 6;                       
    cfg.pin_mosi = 7;                       
    cfg.pin_miso = -1;                      // Not used
    cfg.pin_dc = 16;                        
    _bus_instance.config(cfg);              
    _panel_instance.setBus(&_bus_instance); // Attach bus to panel

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
    _panel_instance.setTouch(&_touch_instance); // Attach touch to panel

    setPanel(&_panel_instance);             // Finalize display setup
  }
};

const int IR_RECEIVE_PIN = 11;   // IR receiver pin

LGFX gfx;                        // Display object
Servo myservo;                   // Servo object

// ==========================
// CONTROL VARIABLES
// ==========================
int angleInput = 0;              // Stores entered angle
bool servoEnabled = true;        // Servo ON/OFF flag

// ==========================
// DISPLAY FUNCTIONS
// ==========================
void showAngleInput() {
  gfx.fillScreen(TFT_BLACK);     // Clear screen
  gfx.setTextSize(3);            // Text size
  gfx.setCursor(30, 100);        // Cursor position
  gfx.setTextColor(TFT_WHITE);   // White text
  gfx.printf("Input: %d", angleInput); // Show entered value
}

void showRotatedAngle(int angle) {
  gfx.fillScreen(TFT_BLACK);     // Clear screen
  gfx.setCursor(30, 100);        
  gfx.setTextSize(3);            
  gfx.setTextColor(TFT_GREEN);   // Green text
  gfx.printf("Rotated to: %d", angle); // Show final angle
}

// ==========================
// SETUP
// ==========================
void setup() {

  Serial.begin(115200);                     // Start serial monitor
  myservo.attach(13, 450, 2520);            // Attach servo (pin + pulse range)
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start IR receiver

  gfx.init();                               // Initialize display
  pinMode(0, OUTPUT);                       // Backlight control pin
  digitalWrite(0, HIGH);                    // Turn ON backlight

  showAngleInput();                         // Show initial screen
}

// ==========================
// MAIN LOOP
// ==========================
void loop() {

  if (IrReceiver.decode()) {                // If IR signal received

    unsigned long code = IrReceiver.decodedIRData.command; // Read IR code

    // ==========================
    // DEBOUNCE LOGIC
    // ==========================
    static uint32_t lastKeyCode = 0;        // Last key pressed
    static unsigned long lastKeyTime = 0;   // Time of last key
    const unsigned long debounceDelay = 300; // Delay (ms)

    if (code == lastKeyCode && (millis() - lastKeyTime) < debounceDelay) {
      IrReceiver.resume();                 // Ignore repeated signal
      return;
    }

    lastKeyCode = code;                    
    lastKeyTime = millis();                

    // ==========================
    // COMMAND HANDLING
    // ==========================
    switch (code) {

      case 0x45:                          // CH- → Disable servo
        servoEnabled = false;
        Serial.println("Servo disabled");
        break;

      case 0x47:                          // CH+ → Enable servo
        servoEnabled = true;
        Serial.println("Servo enabled");
        break;

      case 0x43:                          // PLAY → Confirm & rotate
        angleInput = constrain(angleInput, 0, 180); // Limit angle
        if (servoEnabled) {
          myservo.write(angleInput);      // Move servo
          showRotatedAngle(angleInput);   // Show result
        } else {
          Serial.println("Servo is disabled. Cannot rotate.");
        }
        angleInput = 0;                   // Reset input
        delay(1000);                      // Wait 1 sec
        break;

      // ==========================
      // NUMBER INPUT (0–9)
      // ==========================
      case 0x16: angleInput = angleInput * 10 + 0; break;
      case 0x0C: angleInput = angleInput * 10 + 1; break;
      case 0x18: angleInput = angleInput * 10 + 2; break;
      case 0x5E: angleInput = angleInput * 10 + 3; break;
      case 0x08: angleInput = angleInput * 10 + 4; break;
      case 0x1C: angleInput = angleInput * 10 + 5; break;
      case 0x5A: angleInput = angleInput * 10 + 6; break;
      case 0x42: angleInput = angleInput * 10 + 7; break;
      case 0x52: angleInput = angleInput * 10 + 8; break;
      case 0x4A: angleInput = angleInput * 10 + 9; break;

      default:
        Serial.print("Unknown code: ");
        Serial.println(code, HEX);        // Print unknown IR code
        break;
    }

    // ==========================
    // INPUT LIMIT CHECK
    // ==========================
    if (angleInput > 999) angleInput = 0; // Reset if too large

    showAngleInput();                     // Update display

    IrReceiver.resume();                  // Ready for next signal
  }
}