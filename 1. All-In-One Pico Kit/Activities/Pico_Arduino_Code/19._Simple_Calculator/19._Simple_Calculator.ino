/*
  =========================================================
   Arduino Line Follower Robot (3 IR Sensors)
  =========================================================

  CONNECTIONS:
  ---------------------------------------------------------
  IR Sensors:
    LEFT   -> Pin 2
    MIDDLE -> Pin 3
    RIGHT  -> Pin 4

  Motor Driver (L298N / similar):
    ENA -> 9   (Left motor speed)
    IN1 -> 8
    IN2 -> 7
    IN3 -> 6
    IN4 -> 5
    ENB -> 10  (Right motor speed)

  FUNCTION:
  ---------------------------------------------------------
  - Reads 3 IR sensors
  - Follows black line based on sensor input
  - Adjusts direction (left/right/forward)
*/

 // ==========================
 // IR SENSOR PINS
 // ==========================
#define LEFT_SENSOR 2          // Left IR sensor
#define MID_SENSOR 3           // Middle IR sensor
#define RIGHT_SENSOR 4         // Right IR sensor

// ==========================
// MOTOR DRIVER PINS
// ==========================
#define ENA 9                  // Left motor speed control
#define IN1 8                  // Left motor direction
#define IN2 7
#define IN3 6                  // Right motor direction
#define IN4 5
#define ENB 10                 // Right motor speed control

// ==========================
// SPEED SETTINGS
// ==========================
int speedLeft = 130;           // Base speed for left motor
int speedRight = 130;          // Base speed for right motor

void setup()
{
  // --------------------------
  // SENSOR PIN MODES
  // --------------------------
  pinMode(LEFT_SENSOR, INPUT);   // Left sensor as input
  pinMode(MID_SENSOR, INPUT);    // Middle sensor as input
  pinMode(RIGHT_SENSOR, INPUT);  // Right sensor as input

  // --------------------------
  // MOTOR PIN MODES
  // --------------------------
  pinMode(ENA, OUTPUT);          // Enable left motor
  pinMode(IN1, OUTPUT);          
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);          // Enable right motor

  Serial.begin(9600);            // Start serial monitor
}

void loop()
{
  // ==========================
  // READ SENSOR VALUES
  // ==========================
  int L = digitalRead(LEFT_SENSOR);   // Read left sensor
  int M = digitalRead(MID_SENSOR);    // Read middle sensor
  int R = digitalRead(RIGHT_SENSOR);  // Read right sensor

  // Print sensor values for debugging
  Serial.print(L);
  Serial.print(" ");
  Serial.print(M);
  Serial.print(" ");
  Serial.println(R);

  // ==========================
  // DECISION LOGIC
  // ==========================

  // Forward (center aligned)
  if (L == 1 && M == 0 && R == 1)
  {
    forward();                         // Move straight
  }

  // Turn Left (slight)
  else if (L == 0 && M == 1 && R == 1)
  {
    left();                            // Adjust left
  }

  // Sharp Left
  else if (L == 0 && M == 0 && R == 1)
  {
    left();                            // Strong left turn
  }

  // Turn Right (slight)
  else if (L == 1 && M == 1 && R == 0)
  {
    right();                           // Adjust right
  }

  // Sharp Right
  else if (L == 1 && M == 0 && R == 0)
  {
    right();                           // Strong right turn
  }

  // Default condition
  else
  {
    forward();                         // Keep moving forward
  }
}

// ==========================
// MOTOR FUNCTIONS
// ==========================

void forward()
{
  analogWrite(ENA, speedLeft);        // Set left motor speed
  analogWrite(ENB, speedRight);       // Set right motor speed

  digitalWrite(IN1, LOW);             // Left motor forward
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);             // Right motor forward
  digitalWrite(IN4, HIGH);
}

void left()
{
  analogWrite(ENA, 90);               // Reduce left motor speed
  analogWrite(ENB, 140);              // Increase right motor speed

  digitalWrite(IN1, HIGH);            // Left motor reverse
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);             // Right motor forward
  digitalWrite(IN4, HIGH);
}

void right()
{
  analogWrite(ENA, 140);              // Increase left motor speed
  analogWrite(ENB, 90);               // Reduce right motor speed

  digitalWrite(IN1, LOW);             // Left motor forward
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);            // Right motor reverse
  digitalWrite(IN4, LOW);
}