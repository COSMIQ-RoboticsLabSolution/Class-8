/*
  =========================================================
   Arduino UNO / ESP Board - LED Blinking (Non-Blocking)
   Method Used: millis() (No delay)
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  Red LED:
    Anode (+)  -> Pin 18 (via resistor)
    Cathode (-)-> GND

  Yellow LED:
    Anode (+)  -> Pin 20 (via resistor)
    Cathode (-)-> GND

  Green LED:
    Anode (+)  -> Pin 19 (via resistor)
    Cathode (-)-> GND

  WORKING:
  ---------------------------------------------------------
  - All three LEDs toggle (ON/OFF) every 1 second
  - Uses millis() instead of delay() for non-blocking timing
  - This allows smoother and scalable code
*/

// ==========================
// LIBRARIES
// ==========================
#include <Arduino.h>

// ==========================
// PIN DEFINITIONS
// ==========================
#define Red_LED     18   // Red LED connected to pin 18
#define Yellow_LED  20   // Yellow LED connected to pin 20
#define Green_LED   19   // Green LED connected to pin 19

// ==========================
// TIMING VARIABLES
// ==========================
unsigned long previousMillis = 0;   // Stores last update time
const long interval = 1000;         // Interval = 1 second (1000 ms)

// ==========================
// LED STATE VARIABLES
// ==========================
int redState = LOW;       // Current state of Red LED
int yellowState = LOW;    // Current state of Yellow LED
int greenState = LOW;     // Current state of Green LED

// ==========================
// SETUP FUNCTION
// ==========================
void setup() {

  // Configure LED pins as OUTPUT
  pinMode(Red_LED, OUTPUT);
  pinMode(Yellow_LED, OUTPUT);
  pinMode(Green_LED, OUTPUT);

  // Initialize all LEDs to OFF
  digitalWrite(Red_LED, redState);
  digitalWrite(Yellow_LED, yellowState);
  digitalWrite(Green_LED, greenState);
}

// ==========================
// MAIN LOOP
// ==========================
void loop() {

  // Get current time in milliseconds
  unsigned long currentMillis = millis();

  // Check if defined interval has passed
  if (currentMillis - previousMillis >= interval) {

    // Save the current time
    previousMillis = currentMillis;

    // ==========================
    // TOGGLE LED STATES
    // ==========================
    // If LED is OFF -> turn ON
    // If LED is ON  -> turn OFF

    redState    = (redState == LOW) ? HIGH : LOW;
    yellowState = (yellowState == LOW) ? HIGH : LOW;
    greenState  = (greenState == LOW) ? HIGH : LOW;

    // ==========================
    // APPLY STATES TO PINS
    // ==========================
    digitalWrite(Red_LED, redState);
    digitalWrite(Yellow_LED, yellowState);
    digitalWrite(Green_LED, greenState);
  }
}