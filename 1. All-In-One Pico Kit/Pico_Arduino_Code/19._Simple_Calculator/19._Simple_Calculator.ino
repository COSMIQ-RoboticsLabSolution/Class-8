// IR Sensor Pins
#define LEFT_SENSOR 2
#define MID_SENSOR 3
#define RIGHT_SENSOR 4

// Motor Driver Pins
#define ENA 9
#define IN1 8
#define IN2 7
#define IN3 6
#define IN4 5
#define ENB 10

int speedLeft = 130;
int speedRight = 130;

void setup()
{
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(MID_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  int L = digitalRead(LEFT_SENSOR);
  int M = digitalRead(MID_SENSOR);
  int R = digitalRead(RIGHT_SENSOR);

  Serial.print(L);
  Serial.print(" ");
  Serial.print(M);
  Serial.print(" ");
  Serial.println(R);

  // Forward
  if (L == 1 && M == 0 && R == 1)
  {
    forward();
  }

  // Turn Left
  else if (L == 0 && M == 1 && R == 1)
  {
    left();
  }

  // Sharp Left
  else if (L == 0 && M == 0 && R == 1)
  {
    left();
  }

  // Turn Right
  else if (L == 1 && M == 1 && R == 0)
  {
    right();
  }

  // Sharp Right
  else if (L == 1 && M == 0 && R == 0)
  {
    right();
  }

  else
  {
    forward();
  }
}

void forward()
{
  analogWrite(ENA, speedLeft);
  analogWrite(ENB, speedRight);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left()
{
  analogWrite(ENA, 90);
  analogWrite(ENB, 140);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right()
{
  analogWrite(ENA, 140);
  analogWrite(ENB, 90);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}