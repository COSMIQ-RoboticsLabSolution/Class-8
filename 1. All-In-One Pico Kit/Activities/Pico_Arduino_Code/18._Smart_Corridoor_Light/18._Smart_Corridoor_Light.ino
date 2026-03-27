/*
  =========================================================
   Arduino + BH1750 Light Sensor + Sound Sensor + LED Control
   Libraries Used:
     - Wire.h
     - BH1750.h
  =========================================================

  FUNCTION:
  ---------------------------------------------------------
  - Reads ambient light intensity (lux)
  - Detects sound input
  - Controls LED based on light + sound logic

  LOGIC:
  ---------------------------------------------------------
  - If light is strong → LED OFF (priority)
  - If light is dim → sound controls LED
  - LED stays ON for 10 seconds after sound
*/

#include <Wire.h>              // I2C communication library
#include <BH1750.h>            // Light sensor library

// ==========================
// SENSOR OBJECT
// ==========================
BH1750 lightMeter(0x5c);       // BH1750 sensor (I2C address 0x5C)

// ==========================
// PIN DEFINITIONS
// ==========================
#define I2C_SDA 2              // I2C SDA pin
#define I2C_SCL 3              // I2C SCL pin
#define SOUND_PIN 29           // Sound sensor input pin
#define LedPin 18              // LED output pin

void setup() {

    Serial.begin(9600);                        // Start serial communication

    // ==========================
    // I2C INITIALIZATION
    // ==========================
    Wire1.setSDA(I2C_SDA);                     // Set SDA pin
    Wire1.setSCL(I2C_SCL);                     // Set SCL pin
    Wire1.begin();                             // Start I2C bus

    // ==========================
    // LIGHT SENSOR INIT
    // ==========================
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5c, &Wire1)) {
        Serial.println(F("BH1750 begin success")); // Sensor initialized successfully
    } else {
        Serial.println(F("BH1750 init failed"));   // Sensor initialization failed
    }

    // ==========================
    // PIN MODES
    // ==========================
    pinMode(LedPin, OUTPUT);                  // Set LED pin as output
    pinMode(SOUND_PIN, INPUT);                // Set sound sensor pin as input
}

void loop() {

    // ==========================
    // STATE VARIABLES
    // ==========================
    static bool isSoundDetected = false;      // Tracks if sound was detected
    static bool isLedOn = false;              // Tracks LED state
    static unsigned long ledOnTimestamp = 0;  // Time when LED turned ON

    // ==========================
    // LIGHT SENSOR CHECK
    // ==========================
    if (lightMeter.measurementReady(true)) {  // Check if new light data is ready

        int lux = lightMeter.readLightLevel(); // Read light intensity

        Serial.print("Light Intensity: ");
        Serial.print(lux);
        Serial.println(" lx");

        // ==========================
        // STRONG LIGHT CONDITION
        // ==========================
        if (lux >= 700) {

            Serial.println("[ACTION] Light strong - turning OFF LED");

            digitalWrite(LedPin, LOW);        // Force LED OFF
            isLedOn = false;                  // Update state
            isSoundDetected = false;          // Reset sound flag
            ledOnTimestamp = 0;               // Reset timer
        }

        // ==========================
        // DIM LIGHT CONDITION
        // ==========================
        else {

            if (digitalRead(SOUND_PIN)) {     // If sound detected

                Serial.println("[SOUND] Sound detected - turning ON LED");

                digitalWrite(LedPin, HIGH);   // Turn ON LED
                isLedOn = true;               // Update state
                isSoundDetected = true;       // Mark sound detected
                ledOnTimestamp = millis();    // Save ON time
            } 

            else {

                if (isSoundDetected) {        // If sound was previously detected

                    // --------------------------
                    // KEEP LED ON FOR 10 SECONDS
                    // --------------------------
                    if (millis() - ledOnTimestamp < 10000) {

                        Serial.println("[ACTION] Within 10s - keeping LED ON");

                        digitalWrite(LedPin, HIGH); // Keep LED ON
                        isLedOn = true;
                    } 

                    else {

                        Serial.println("[ACTION] 10s passed - turning OFF LED");

                        digitalWrite(LedPin, LOW); // Turn OFF LED
                        isLedOn = false;
                        isSoundDetected = false;   // Reset flag
                    }
                } 

                else {

                    Serial.println("[ACTION] No recent sound - turning OFF LED");

                    digitalWrite(LedPin, LOW); // Ensure LED OFF
                    isLedOn = false;
                }
            }
        }
    }

    delay(10); // Small delay to reduce CPU usage
}