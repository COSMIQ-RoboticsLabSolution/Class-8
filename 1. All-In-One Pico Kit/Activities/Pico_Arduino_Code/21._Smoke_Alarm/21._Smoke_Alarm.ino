/*
  =========================================================
   Arduino + MQ2 Gas Sensor + Buzzer Alert System
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  MQ2 Gas Sensor:
    VCC -> 5V
    GND -> GND
    AOUT -> Pin 26

  Buzzer:
    + -> Pin 10
    - -> GND

  FUNCTION:
  ---------------------------------------------------------
  - Reads gas/smoke level using MQ2 sensor
  - Converts analog value to voltage
  - If voltage > threshold → buzzer ON
  - Else → buzzer OFF
*/

#define gas_pin 26              // MQ2 analog output pin
const int buzzerPin = 10;       // Buzzer pin

void setup() {

  pinMode(buzzerPin, OUTPUT);   // Set buzzer pin as output

  Serial.begin(115200);         // Start serial communication

  Serial.println("MQ2 ready!"); // Initialization message
}

void loop() {

  float sensorValue = analogRead(gas_pin);      // Read analog value from MQ2 sensor

  float sensor_volt = sensorValue / 1023.0 * 5.0; // Convert analog reading to voltage (0–5V)

  Serial.print("Voltage: ");                   // Print label
  Serial.print(sensor_volt);                   // Print voltage value
  Serial.println(" V");                        // Print unit

  // ==========================
  // GAS DETECTION LOGIC
  // ==========================
  if (sensor_volt > 1.0) {                    // Threshold check for gas/smoke detection

    tone(buzzerPin, 1300);                   // Turn ON buzzer at 1300 Hz
  } 
  else {

    noTone(buzzerPin);                       // Turn OFF buzzer
  }

  delay(100);                                // Small delay for stability
}