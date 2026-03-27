/*
  =========================================================
   RP2040 (Slider Controlled LED Brightness)
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  Slider (Potentiometer):
    VCC  -> 3.3V / 5V
    GND  -> GND
    OUT  -> GPIO 28 (ADC2)

  LEDs (PWM Control):
    Red LED    -> GPIO 18
    Yellow LED -> GPIO 20
    Green LED  -> GPIO 19

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Slider controls brightness of all LEDs simultaneously
  - ADC reads analog value (0–4095)
  - Value is mapped to PWM range (0–255)
  - Same brightness applied to all LEDs
*/

#include <Arduino.h>

// ==========================
// PIN DEFINITIONS
// ==========================
#define SLIDER_PIN     28   // Slider connected to ADC pin
#define RED_LED_PIN    18   // Red LED (PWM capable)
#define YELLOW_LED_PIN 20   // Yellow LED (PWM capable)
#define GREEN_LED_PIN  19   // Green LED (PWM capable)

void setup() {

  // ==========================
  // SERIAL INITIALIZATION
  // ==========================
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection (optional)

  // ==========================
  // PIN CONFIGURATION
  // ==========================
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // ==========================
  // SYSTEM READY MESSAGE
  // ==========================
  Serial.println("LED Brightness Control with Slider Ready.");
}

void loop() {

  // ==========================
  // READ SLIDER VALUE (ADC)
  // Range: 0 – 4095 (12-bit)
  // ==========================
  int analogValue = analogRead(SLIDER_PIN);

  // ==========================
  // MAP ADC TO PWM RANGE
  // PWM Range: 0 – 255
  // ==========================
  int pwmValue = map(analogValue, 0, 4095, 0, 255);

  // ==========================
  // CONTROL LED BRIGHTNESS
  // All LEDs get same brightness
  // ==========================
  analogWrite(RED_LED_PIN, pwmValue);
  analogWrite(YELLOW_LED_PIN, pwmValue);
  analogWrite(GREEN_LED_PIN, pwmValue);

  // ==========================
  // DEBUG OUTPUT
  // ==========================
  Serial.print("ADC: ");
  Serial.print(analogValue);

  Serial.print(" | PWM: ");
  Serial.println(pwmValue);

  // ==========================
  // DELAY FOR STABILITY
  // ==========================
  delay(100);
}