/*
  =========================================================
   RP2040 + BH1750 Light Sensor + LED Control
  =========================================================

  LIBRARIES USED:
  ---------------------------------------------------------
  - Wire (I2C communication)
  - BH1750 (Light intensity sensor)

  CONNECTIONS:
  ---------------------------------------------------------
  BH1750 Light Sensor (I2C):
    VCC -> 3.3V / 5V
    GND -> GND
    SDA -> GPIO 2
    SCL -> GPIO 3
    Address -> 0x5C

  LED:
    Red LED -> GPIO 18

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Reads ambient light intensity (in lux)
  - If light < 100 lux → LED ON
  - If light ≥ 100 lux → LED OFF
*/

#include <Wire.h>     // I2C communication library
#include <BH1750.h>   // BH1750 sensor library

// ==========================
// I2C PIN DEFINITIONS
// ==========================
#define I2C_SDA 2
#define I2C_SCL 3

// ==========================
// LED PIN DEFINITION
// ==========================
#define Red_LED 18

// ==========================
// BH1750 SENSOR OBJECT
// ==========================
BH1750 lightMeter(0x5c); // Sensor with I2C address 0x5C

void setup() {

  // ==========================
  // SERIAL INITIALIZATION
  // ==========================
  Serial.begin(9600);
  delay(100); // Allow serial to stabilize

  // ==========================
  // I2C INITIALIZATION
  // ==========================
  Wire1.setSDA(I2C_SDA);
  Wire1.setSCL(I2C_SCL);
  Wire1.begin();

  // ==========================
  // SENSOR INITIALIZATION
  // ==========================
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5c, &Wire1)) {
    Serial.println("BH1750 sensor initialized.");
  } else {
    Serial.println("Failed to initialize BH1750 sensor.");
  }

  // ==========================
  // LED PIN SETUP
  // ==========================
  pinMode(Red_LED, OUTPUT);
}

void loop() {

  // ==========================
  // CHECK IF SENSOR IS READY
  // ==========================
  if (lightMeter.measurementReady(true)) {

    // Read light intensity in lux
    float lux = lightMeter.readLightLevel();

    // ==========================
    // SERIAL OUTPUT
    // ==========================
    Serial.print("Current light level: ");
    Serial.print(lux);
    Serial.println(" lx");

    // ==========================
    // LED CONTROL LOGIC
    // ==========================
    if (lux < 100) {
      digitalWrite(Red_LED, HIGH); // Low light → LED ON
    } else {
      digitalWrite(Red_LED, LOW);  // Bright light → LED OFF
    }
  }

  // ==========================
  // DELAY FOR STABILITY
  // ==========================
  delay(200);
}