/*
  =========================================================
   Arduino + LVGL + ST7789 Touch Display + DHT20 Monitor
   Libraries Used:
     - lvgl.h
     - LovyanGFX.hpp
     - DHT20.h
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Displays Temperature & Humidity on TFT Touch Screen
  - Uses LVGL for GUI
  - Shows alerts based on conditions
  - Controls LEDs and buzzer

  FEATURES:
  ---------------------------------------------------------
  - Real-time sensor monitoring
  - Touch-enabled UI
  - LED indicators (Hot / Normal)
  - Buzzer alert for low humidity
*/

#include <lvgl.h>                 // LVGL graphics library
// #include "demos/lv_demos.h"   // Optional demo (commented)
#include <Wire.h>                 // I2C communication
#include "ui.h"                  // UI elements generated via LVGL
#include <Arduino.h>             // Core Arduino functions
#include <SPI.h>                 // SPI communication
#include "DHT20.h"              // DHT20 sensor library
#include <LovyanGFX.hpp>         // Display control library

/* Screen resolution and rotation settings */
#define TFT_HOR_RES   320        // Horizontal resolution
#define TFT_VER_RES   240        // Vertical resolution
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0  // Display rotation

/* LVGL drawing buffer size */
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8)) // Buffer size calculation
uint32_t draw_buf[DRAW_BUF_SIZE / 4]; // LVGL buffer array

/* Custom LGFX class for ST7789 display + touch */
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel;     // Display panel object
  lgfx::Bus_SPI _bus;            // SPI bus object
  lgfx::Touch_FT5x06 _touch;     // Touch controller

public:
  LGFX() {

    // ==========================
    // SPI BUS CONFIGURATION
    // ==========================
    auto bus_cfg = _bus.config(); 
    bus_cfg.spi_host = 0;         
    bus_cfg.freq_write = 80000000; // SPI frequency
    bus_cfg.pin_sclk = 6;         
    bus_cfg.pin_mosi = 7;         
    bus_cfg.pin_dc = 16;          
    _bus.config(bus_cfg);         
    _panel.setBus(&_bus);         

    // ==========================
    // DISPLAY PANEL CONFIG
    // ==========================
    auto panel_cfg = _panel.config();
    panel_cfg.pin_cs = 17;        
    panel_cfg.memory_width = 240; 
    panel_cfg.memory_height = 320;
    panel_cfg.panel_width = 240;  
    panel_cfg.panel_height = 320; 
    panel_cfg.offset_rotation = 1;
    panel_cfg.invert = true;      

    _panel.config(panel_cfg);     

    // ==========================
    // TOUCH CONFIGURATION
    // ==========================
    auto touch_cfg = _touch.config();
    touch_cfg.x_min = 0;          
    touch_cfg.x_max = 239;        
    touch_cfg.y_min = 0;          
    touch_cfg.y_max = 319;        
    touch_cfg.pin_int = 25;       
    touch_cfg.pin_rst = 24;       
    touch_cfg.i2c_port = 0;       
    touch_cfg.i2c_addr = 0x38;    
    touch_cfg.pin_sda = 4;        
    touch_cfg.pin_scl = 5;        
    touch_cfg.freq = 400000;      

    _touch.config(touch_cfg);     
    _panel.setTouch(&_touch);     

    setPanel(&_panel);            // Attach panel to LGFX
  }
};

LGFX gfx;                         // Display object
DHT20 DHT(&Wire1);               // DHT20 sensor (using Wire1)

unsigned long lastUpdate = 0;    // Last sensor update time
const unsigned long interval = 1000; // Update interval (1 second)

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
    LV_UNUSED(level);             // Ignore log level
    Serial.println(buf);          // Print LVGL logs
    Serial.flush();               // Flush serial buffer
}
#endif

/* Display flush callback (LVGL → screen) */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = area->x2 - area->x1 + 1; // Width of area
    uint32_t h = area->y2 - area->y1 + 1; // Height of area
    
    gfx.setAddrWindow(area->x1, area->y1, w, h); // Set drawing window
    gfx.pushPixels((lgfx::rgb565_t *)px_map, w * h); // Push pixels to display

    lv_display_flush_ready(disp); // Notify LVGL rendering done
}

/* Touch input callback */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;                    
    bool touched = gfx.getTouch(&touchX, &touchY); // Read touch data
    
    if (touched) {
        data->state = LV_INDEV_STATE_PRESSED;  // Touch detected
        data->point.x = touchX;                
        data->point.y = touchY;                
    } else {
        data->state = LV_INDEV_STATE_RELEASED; // No touch
    }
}

/* LVGL tick function */
static uint32_t my_tick(void)
{
    return millis(); // Use Arduino millis as LVGL clock
}

void setup(void) {

  Serial.begin(115200); // Start serial communication
  Serial.println("Starting Temperature and Humidity Monitor...");

  // ==========================
  // I2C INITIALIZATION
  // ==========================
  Wire.setSDA(4);       
  Wire.setSCL(5);       
  Wire.begin();         
  delay(100);           

  Wire1.setSDA(2);      
  Wire1.setSCL(3);      
  Wire1.begin();        

  // ==========================
  // DISPLAY INITIALIZATION
  // ==========================
  Serial.println("Initializing display...");
  gfx.init();           
  gfx.invertDisplay(false); 
  gfx.setRotation(TFT_ROTATION);

  pinMode(0, OUTPUT);   // Backlight control pin
  digitalWrite(0, HIGH);// Turn ON backlight

  // ==========================
  // LVGL INITIALIZATION
  // ==========================
  lv_init();            
  lv_tick_set_cb(my_tick);

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); // Enable logs
#endif

  lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES); // Create display
  lv_display_set_flush_cb(disp, my_disp_flush); // Set flush callback
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create(); // Create input device
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); 
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // ==========================
  // UI INITIALIZATION
  // ==========================
  ui_init();                                 // Initialize UI
  lv_label_set_text(ui_TempLabel, "Initializing..."); 
  lv_label_set_text(ui_HumiLabel, "Initializing...");
  lv_label_set_text(ui_AlarmLabel, "");

  // ==========================
  // SENSOR INITIALIZATION
  // ==========================
  Serial.println("Initializing DHT20 sensor...");
  DHT.begin(); // Start DHT20 sensor

  // ==========================
  // OUTPUT DEVICES
  // ==========================
  pinMode(20, OUTPUT); // Yellow LED
  pinMode(18, OUTPUT); // Red LED
  pinMode(19, OUTPUT); // Green LED
  pinMode(10, OUTPUT); // Buzzer

  digitalWrite(20, LOW); // All OFF initially
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);
  noTone(10);           // Buzzer OFF
}

void loop() {

  lv_timer_handler(); // Handle LVGL tasks

  unsigned long currentMillis = millis(); // Current time

  if (currentMillis - lastUpdate >= interval) { // Check interval

    int status = DHT.read(); // Read sensor

    if (status == DHT20_OK) { // If read successful

      float humidity = DHT.getHumidity();     // Get humidity
      float temperature = DHT.getTemperature(); // Get temperature

      char tempStr[16]; 
      char humiStr[16]; 

      snprintf(tempStr, sizeof(tempStr), "%.1f C", temperature); // Format temp
      snprintf(humiStr, sizeof(humiStr), "%.1f %%", humidity);   // Format humidity

      lv_label_set_text(ui_TempLabel, tempStr); // Update UI
      lv_label_set_text(ui_HumiLabel, humiStr);

      Serial.print("Temp: ");
      Serial.print(temperature);
      Serial.print("  Humi: ");
      Serial.println(humidity);

      // ==========================
      // TEMPERATURE LOGIC
      // ==========================
      if (temperature > 30) {
        digitalWrite(18, HIGH); // Red LED ON
        digitalWrite(20, LOW);  
        lv_label_set_text(ui_AlarmLabel, "It's hot");
      } 
      else if (temperature > 25) {
        digitalWrite(20, HIGH); // Yellow LED ON
        digitalWrite(18, LOW);
        lv_label_set_text(ui_AlarmLabel, "Temperature is high");
      } 
      else {
        digitalWrite(20, LOW);
        digitalWrite(18, LOW);
      }

      // ==========================
      // HUMIDITY LOGIC
      // ==========================
      if (humidity < 40.0) {
        tone(10, 1300); // Buzzer ON
        lv_label_set_text(ui_AlarmLabel, "Air is dry");
      } 
      else {
        noTone(10); // Buzzer OFF

        if (temperature <= 25 && humidity >= 40.0) {
          lv_label_set_text(ui_AlarmLabel, ""); // Clear message
        }
      }
    }

    lastUpdate = currentMillis; // Update timer
  }

  delay(5); // Small delay for stability
}