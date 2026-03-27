/*
  =========================================================
   Arduino UNO + IR Remote + LED Control System
   Library Used: IRremote.h
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  IR Receiver:
    VCC  -> 5V
    GND  -> GND
    OUT  -> Pin 11

  LEDs:
    Red LED    -> Pin 18
    Yellow LED -> Pin 20
    Green LED  -> Pin 19

  FUNCTION:
  ---------------------------------------------------------
  Use IR remote to:
   - Turn ON individual LEDs
   - Blink all LEDs
   - Run LED chasing (running light)
   - Turn OFF all LEDs
*/

#include <IRremote.h>              // Include IRremote library for IR communication

const int IR_RECEIVE_PIN = 11;     // Pin connected to IR receiver

// ==========================
// LED PIN DEFINITIONS
// ==========================
const int RED_LED_PIN = 18;        // Red LED pin
const int YELLOW_LED_PIN = 20;     // Yellow LED pin
const int GREEN_LED_PIN = 19;      // Green LED pin

// ==========================
// EFFECT CONTROL VARIABLES
// ==========================
unsigned long lastEffectTime = 0;  // Stores last update time for effects
int effectMode = 0;                // Current mode (0=off, 1=blink, 2=running)
int effectStep = 0;                // Step counter for running light

void setup() {
  Serial.begin(9600);                                  // Start serial communication
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Initialize IR receiver

  pinMode(RED_LED_PIN, OUTPUT);                        // Set Red LED as output
  pinMode(YELLOW_LED_PIN, OUTPUT);                     // Set Yellow LED as output
  pinMode(GREEN_LED_PIN, OUTPUT);                      // Set Green LED as output

  turnOffAll();                                        // Ensure all LEDs are OFF initially
}

void loop() {

  // ==========================
  // IR SIGNAL HANDLING
  // ==========================
  if (IrReceiver.decode()) {                           // Check if IR signal received
    unsigned long code = IrReceiver.decodedIRData.command; // Read IR command

    Serial.print("Received code: 0x");                 // Print received code
    Serial.println(code, HEX);

    handleIR(code);                                   // Process IR command
    IrReceiver.resume();                              // Prepare for next signal
  }

  // ==========================
  // EFFECT HANDLING
  // ==========================
  handleEffects();                                    // Run lighting effects
}

void handleIR(unsigned long code) {

  turnOffAll();                                       // Turn OFF all LEDs first
  effectMode = 0;                                     // Reset any running effect

  switch (code) {

    case 0x0C:                                        // Button 1
      digitalWrite(RED_LED_PIN, HIGH);                // Turn ON Red LED
      Serial.println("Red light on");
      break;

    case 0x18:                                        // Button 2
      digitalWrite(YELLOW_LED_PIN, HIGH);             // Turn ON Yellow LED
      Serial.println("Yellow light on");
      break;

    case 0x5E:                                        // Button 3
      digitalWrite(GREEN_LED_PIN, HIGH);              // Turn ON Green LED
      Serial.println("Green light on");
      break;

    case 0x08:                                        // Button 4
      effectMode = 1;                                 // Enable blinking mode
      Serial.println("Start blinking");
      break;

    case 0x1C:                                        // Button 5
      effectMode = 2;                                 // Enable running light mode
      Serial.println("Start running light");
      break;

    case 0x5A:                                        // Button 6
      Serial.println("All lights off");               // Just keep LEDs OFF
      break;
  }
}

void handleEffects() {

  // ==========================
  // BLINK MODE
  // ==========================
  if (effectMode == 1) {

    if (millis() - lastEffectTime > 300) {             // Check delay (300 ms)
      static bool on = false;                          // Toggle state variable
      on = !on;                                       // Switch ON/OFF

      digitalWrite(RED_LED_PIN, on);                  // Apply state to Red LED
      digitalWrite(YELLOW_LED_PIN, on);               // Apply state to Yellow LED
      digitalWrite(GREEN_LED_PIN, on);                // Apply state to Green LED

      lastEffectTime = millis();                      // Update timer
    }
  }

  // ==========================
  // RUNNING LIGHT MODE
  // ==========================
  else if (effectMode == 2) {

    if (millis() - lastEffectTime > 300) {             // Check delay (300 ms)

      switch (effectStep % 3) {                        // Cycle through LEDs

        case 0:
          digitalWrite(RED_LED_PIN, HIGH);            // Red ON
          digitalWrite(YELLOW_LED_PIN, LOW);          // Yellow OFF
          digitalWrite(GREEN_LED_PIN, LOW);           // Green OFF
          break;

        case 1:
          digitalWrite(RED_LED_PIN, LOW);             // Red OFF
          digitalWrite(YELLOW_LED_PIN, HIGH);         // Yellow ON
          digitalWrite(GREEN_LED_PIN, LOW);           // Green OFF
          break;

        case 2:
          digitalWrite(RED_LED_PIN, LOW);             // Red OFF
          digitalWrite(YELLOW_LED_PIN, LOW);          // Yellow OFF
          digitalWrite(GREEN_LED_PIN, HIGH);          // Green ON
          break;
      }

      effectStep++;                                   // Move to next step
      lastEffectTime = millis();                      // Update timer
    }
  }
}

void turnOffAll() {
  digitalWrite(RED_LED_PIN, LOW);     // Turn OFF Red LED
  digitalWrite(YELLOW_LED_PIN, LOW);  // Turn OFF Yellow LED
  digitalWrite(GREEN_LED_PIN, LOW);   // Turn OFF Green LED
}