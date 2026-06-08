#include <Wire.h>
#include <BH1750.h>

#define RED_LED 18   // LED pin

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(10);
  }

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);

  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C, &Wire)) {
    Serial.println("BH1750 initialized successfully");
  } else {
    Serial.println("BH1750 initialization failed");
    while (1);
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();

  Serial.print("Lux: ");
  Serial.print(lux);
  Serial.println(" lx");

  // Turn LED ON when it is dark
  if (lux < 100) {
    digitalWrite(RED_LED, HIGH);
    Serial.println("LED ON (Low Light)");
  }
  else {
    digitalWrite(RED_LED, LOW);
    Serial.println("LED OFF (Bright Light)");
  }

  Serial.println("--------------------");

  delay(1000);
}
