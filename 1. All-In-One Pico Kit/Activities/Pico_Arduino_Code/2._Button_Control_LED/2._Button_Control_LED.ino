/*
  =========================================================
   Arduino (Analog Button + LEDs Control)
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  LEDs:
    Red LED    -> Pin 18
    Yellow LED -> Pin 20
    Green LED  -> Pin 19

  Analog Button Module:
    Output -> Pin 27 (ADC Pin)

  NOTE:
  ---------------------------------------------------------
  - Multiple buttons are connected using a resistor ladder
  - Each button gives a different ADC value range
  - We detect button press using ADC ranges
  - Median filter is used to remove noise

  FUNCTIONALITY:
  ---------------------------------------------------------
  Button 0 -> Toggle ALL LEDs
  Button 1 -> Toggle RED LED
  Button 2 -> Toggle YELLOW LED
  Button 3 -> Toggle GREEN LED
*/

#include <Arduino.h>

// ==========================
// LED PIN DEFINITIONS
// ==========================
#define Red_LED    18
#define Yellow_LED 20
#define Green_LED  19

// ==========================
// ANALOG BUTTON PIN
// ==========================
#define Button_pin 27

// ==========================
// ADC RANGES FOR BUTTONS
// (Adjust as per your hardware)
// ==========================
#define B0_L 740
#define B0_H 750

#define B1_L 800
#define B1_H 810

#define B2_L 860
#define B2_H 870

#define B3_L 900
#define B3_H 920

// ==========================
// LED STATE VARIABLES
// ==========================
bool redOn = false;
bool yellowOn = false;
bool greenOn = false;
bool allOn = false;

// ==========================
// DEBOUNCE FLAG
// Prevents multiple triggers
// ==========================
bool keyIsPressed = false;

/*
  =========================================================
   FUNCTION: readMedianADC
  =========================================================
  - Reads analog value 5 times
  - Sorts readings
  - Returns the middle (median) value
  - Helps reduce noise in ADC readings
*/
int readMedianADC(int pin) {
  int readings[5];

  // Take 5 samples
  for (int i = 0; i < 5; i++) {
    readings[i] = analogRead(pin);
    delay(3); // Small delay between samples
  }
  
  // Sort values using bubble sort
  for (int i = 0; i < 4; i++) {
    for (int j = i + 1; j < 5; j++) {
      if (readings[i] > readings[j]) {
        int temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }
  
  // Return median value
  return readings[2];
}

void setup() {
  Serial.begin(115200); // Start serial communication
  
  // ==========================
  // PIN CONFIGURATION
  // ==========================
  pinMode(Red_LED, OUTPUT);
  pinMode(Yellow_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);
  pinMode(Button_pin, INPUT);
  
  // ==========================
  // INITIAL STATE (ALL OFF)
  // ==========================
  digitalWrite(Red_LED, LOW);
  digitalWrite(Yellow_LED, LOW);
  digitalWrite(Green_LED, LOW);
  
  Serial.println("System Ready.");
}

void loop() {

  // ==========================
  // READ ANALOG VALUE
  // ==========================
  int adcValue = readMedianADC(Button_pin);

  Serial.print("ADC: ");
  Serial.println(adcValue); // Debugging output
  
  // ==========================
  // CHECK IF VALUE MATCHES ANY BUTTON RANGE
  // ==========================
  bool inRange =
    (adcValue >= B0_L && adcValue <= B0_H) ||
    (adcValue >= B1_L && adcValue <= B1_H) ||
    (adcValue >= B2_L && adcValue <= B2_H) ||
    (adcValue >= B3_L && adcValue <= B3_H);
  
  // ==========================
  // BUTTON PRESS DETECTED
  // ==========================
  if (inRange && !keyIsPressed) {

    keyIsPressed = true; // Enable debounce
    
    // --------------------------
    // BUTTON 0 → TOGGLE ALL LEDs
    // --------------------------
    if (adcValue >= B0_L && adcValue <= B0_H) {
      allOn = !allOn;

      // Sync all LED states
      redOn = yellowOn = greenOn = allOn;

      digitalWrite(Red_LED, redOn ? HIGH : LOW);
      digitalWrite(Yellow_LED, yellowOn ? HIGH : LOW);
      digitalWrite(Green_LED, greenOn ? HIGH : LOW);

      Serial.println("Button 0: Toggle ALL LEDs");
    }
    
    // --------------------------
    // BUTTON 1 → TOGGLE RED LED
    // --------------------------
    else if (adcValue >= B1_L && adcValue <= B1_H) {
      redOn = !redOn;

      digitalWrite(Red_LED, redOn ? HIGH : LOW);

      Serial.println("Button 1: Toggle RED");
    }
    
    // --------------------------
    // BUTTON 2 → TOGGLE YELLOW LED
    // --------------------------
    else if (adcValue >= B2_L && adcValue <= B2_H) {
      yellowOn = !yellowOn;

      digitalWrite(Yellow_LED, yellowOn ? HIGH : LOW);

      Serial.println("Button 2: Toggle YELLOW");
    }
    
    // --------------------------
    // BUTTON 3 → TOGGLE GREEN LED
    // --------------------------
    else if (adcValue >= B3_L && adcValue <= B3_H) {
      greenOn = !greenOn;

      digitalWrite(Green_LED, greenOn ? HIGH : LOW);

      Serial.println("Button 3: Toggle GREEN");
    }
  }
  
  // ==========================
  // RESET DEBOUNCE (BUTTON RELEASE)
  // ==========================
  if (!inRange && keyIsPressed) {
    keyIsPressed = false;
  }
  
  delay(20); // Small delay for stability
}