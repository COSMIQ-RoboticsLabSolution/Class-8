from machine import Pin, PWM
from utime import sleep

# ---------------- PINS ----------------
CollisionPin_L = 17  # Left sensor
CollisionPin_R = 18  # Right sensor

MotorPinA_1A = 10
MotorPinA_1B = 11
MotorPinB_1A = 12
MotorPinB_1B = 13

# ---------------- SETUP ----------------
def setup():
    global motorA1, motorA2, motorB1, motorB2
    global Collision_L, Collision_R

    motorA1 = PWM(Pin(MotorPinA_1A))
    motorA2 = PWM(Pin(MotorPinA_1B))
    motorB1 = PWM(Pin(MotorPinB_1A))
    motorB2 = PWM(Pin(MotorPinB_1B))

    # Set PWM frequency
    motorA1.freq(1000)
    motorA2.freq(1000)
    motorB1.freq(1000)
    motorB2.freq(1000)

    Collision_L = Pin(CollisionPin_L, Pin.IN, Pin.PULL_UP)
    Collision_R = Pin(CollisionPin_R, Pin.IN, Pin.PULL_UP)

# ---------------- MOTOR CONTROL ----------------
def motor(A1, A2, B1, B2):
    motorA1.duty_u16(A1)
    motorA2.duty_u16(A2)
    motorB1.duty_u16(B1)
    motorB2.duty_u16(B2)

# ---------------- LOOP ----------------
def loop():
    speed = 50000

    while True:
        Sum = Collision_L.value() * 2 + Collision_R.value()
        print("Sensor:", Sum)

        sleep(0.01)

        # 0 = no collision
        if Sum == 0:
            motor(0, speed, 0, speed)  # forward

        # 1 = right collision
        elif Sum == 1:
            motor(speed, 0, speed, 0)  # backward
            sleep(2)
            motor(speed, 0, 0, speed)  # turn left
            sleep(2)

        # 2 = left collision
        elif Sum == 2:
            motor(speed, 0, speed, 0)  # backward
            sleep(2)
            motor(0, speed, speed, 0)  # turn right
            sleep(2)

        # 3 = both collision
        elif Sum == 3:
            motor(speed, 0, speed, 0)  # backward
            sleep(2)
            motor(speed, 0, 0, speed)  # U-turn (left)
            sleep(2)

# ---------------- MAIN ----------------
if __name__ == '__main__':
    setup()
    loop()