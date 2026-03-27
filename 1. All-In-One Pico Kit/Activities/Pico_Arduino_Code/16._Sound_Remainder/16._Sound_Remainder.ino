/*
  =========================================================
   Arduino + Sound Sensor + Buzzer Alert System
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  Sound Sensor:
    VCC -> 5V
    GND -> GND
    OUT -> Pin 29 (Analog)

  Buzzer:
    + -> Pin 10
    - -> GND

  FUNCTION:
  ---------------------------------------------------------
  - Reads sound intensity using analog sensor
  - If sound exceeds threshold → buzzer ON
  - Else → buzzer OFF
*/

#define SOUND_PIN 29            // Sound sensor analog input pin
int buzzerPin = 10;             // Buzzer output pin

void setup() {

  pinMode(SOUND_PIN, INPUT);    // Configure sound sensor pin as input
  pinMode(buzzerPin, OUTPUT);   // Configure buzzer pin as output

  Serial.begin(9600);           // Start serial communication
  Serial.println("Sound detection started..."); // Startup message
}

void loop() {

  int soundValue = analogRead(SOUND_PIN);  // Read analog value from sound sensor

  Serial.print("Sound Value: ");           // Print label
  Serial.println(soundValue);              // Print sensor value

  // ==========================
  // SOUND DETECTION LOGIC
  // ==========================
  if (soundValue >= 300) {                 // Check if sound exceeds threshold

    Serial.println("Loud sound detected! Beep!"); // Alert message

    tone(buzzerPin, 1300);                // Activate buzzer at 1300 Hz
    delay(1000);                          // Keep buzzer ON for 1 second
  } 
  else {

    Serial.println("Quiet. Buzzer off."); // Quiet condition message

    noTone(buzzerPin);                    // Turn OFF buzzer
  }

  delay(100);                             // Small delay for stability
}