/*
  =========================================================
   Arduino UNO + Servo Motor Sweep
   Library Used: Servo.h
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  Servo Motor:
    VCC  -> 5V
    GND  -> GND
    Signal -> Pin 13

  FUNCTION:
  ---------------------------------------------------------
  Servo moves from 0° to 180° and back continuously
*/

#include <Servo.h>                 // Include Servo library to control servo motor

Servo myservo;                     // Create servo object to control the servo

void setup() {
  myservo.attach(13, 450, 2520);  // Attach servo to pin 13 with pulse width range (450–2520 microseconds)
}

void loop() {

  // ==========================
  // FORWARD MOVEMENT (0° → 180°)
  // ==========================
  for (int pos = 0; pos <= 180; pos++) {   // Loop from 0 to 180 degrees
    myservo.write(pos);                   // Set servo angle to current position
    delay(15);                           // Delay to control speed of movement
  }

  // ==========================
  // BACKWARD MOVEMENT (180° → 0°)
  // ==========================
  for (int pos = 180; pos >= 0; pos--) { // Loop from 180 back to 0 degrees
    myservo.write(pos);                  // Set servo angle to current position
    delay(15);                          // Delay to control speed of movement
  }
}