from machine import Pin
from utime import sleep

# Motor Pins
MotorPinA_1A = 10
MotorPinA_1B = 11
MotorPinB_1A = 12
MotorPinB_1B = 13

# ---------------- SETUP ----------------
def setup():
    global motorA1, motorA2, motorB1, motorB2

    motorA1 = Pin(MotorPinA_1A, Pin.OUT)
    motorA2 = Pin(MotorPinA_1B, Pin.OUT)
    motorB1 = Pin(MotorPinB_1A, Pin.OUT)
    motorB2 = Pin(MotorPinB_1B, Pin.OUT)

# ---------------- MOTOR FUNCTION ----------------
def motor(A1, A2, B1, B2):
    motorA1.value(A1)
    motorA2.value(A2)
    motorB1.value(B1)
    motorB2.value(B2)

# ---------------- LOOP ----------------
def loop():
    while True:
        # Forward
        motor(1, 0, 1, 0)
        sleep(2)

        # Backward
        motor(0, 1, 0, 1)
        sleep(2)

        # Left turn
        motor(0, 1, 1, 0)
        sleep(2)

        # Right turn
        motor(1, 0, 0, 1)
        sleep(2)

        # Stop
        motor(0, 0, 0, 0)
        sleep(1)

# ---------------- MAIN ----------------
if __name__ == '__main__':
    setup()
    loop()