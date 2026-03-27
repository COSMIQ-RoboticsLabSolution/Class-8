/*
  =========================================================
   Ultrasonic Sensor + Vibration Motor Alert System
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  Ultrasonic Sensor (HC-SR04):
    TRIG -> Pin 9
    ECHO -> Pin 8
    VCC  -> 5V
    GND  -> GND

  Vibration Motor:
    Signal -> Pin 15

  FUNCTIONALITY:
  ---------------------------------------------------------
  - Measures distance continuously
  - If object is closer than 30 cm → vibration ON
  - If safe distance → vibration OFF
  - Prints status on Serial Monitor
*/

#include <HCSR04.h> // Library for ultrasonic sensor

// ==========================
// PIN DEFINITIONS
// ==========================
const byte triggerPin = 9;     // Trigger pin of ultrasonic sensor
const byte echoPin = 8;        // Echo pin of ultrasonic sensor
const int vibratePin = 15;     // Vibration motor control pin

// ==========================
// SENSOR OBJECT
// ==========================
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin); // Create ultrasonic sensor instance

// ==========================
// TIMING CONTROL
// ==========================
const unsigned long UPDATE_INTERVAL = 100; // Time between readings (ms)
unsigned long lastUpdateTime = 0;          // Stores last update time

void setup() {

  pinMode(vibratePin, OUTPUT);    // Set vibration motor pin as output
  digitalWrite(vibratePin, LOW);  // Keep motor OFF initially

  Serial.begin(9600);             // Start serial communication
  while (!Serial);                // Wait for serial (for some boards)

  Serial.println("System ready. Starting distance monitoring..."); // Startup message
}

void loop() {

  unsigned long currentTime = millis(); // Get current time

  // Check if it's time to take a new reading
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {

    lastUpdateTime = currentTime; // Update last read time

    float distance = distanceSensor.measureDistanceCm(); // Measure distance in cm

    // ==========================
    // VALID MEASUREMENT CHECK
    // ==========================
    if (distance > 0) { // Valid reading

      Serial.print("Distance: "); // Print label
      Serial.print(distance);     // Print value
      Serial.println(" cm");      // Print unit

      // ==========================
      // OBSTACLE DETECTION LOGIC
      // ==========================
      if (distance < 30.0) { // Object is near

        digitalWrite(vibratePin, HIGH); // Turn ON vibration motor
        Serial.println("Obstacle detected! Vibration motor ON."); // Alert message

      } else {

        digitalWrite(vibratePin, LOW); // Turn OFF motor
        Serial.println("Safe distance. Vibration motor OFF."); // Safe message
      }

    } else {

      // ==========================
      // INVALID MEASUREMENT
      // ==========================
      digitalWrite(vibratePin, LOW); // Turn OFF motor for safety
      Serial.println("Out of range or measurement error."); // Error message
    }
  }

  delay(10); // Small delay to reduce CPU load
}