from machine import Pin, ADC
from time import sleep
from servo import Servo

s1 = Servo(0)

VRX = ADC(0)  # GP26
VRY = ADC(1)  # GP27
SW = Pin(28, Pin.IN, Pin.PULL_UP)

def Map(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def servo_Angle(angle):
    s1.goto(round(Map(angle, 0, 180, 0, 1024)))

def direction():
    global i
    i = 0

    adc_X = round(Map(VRX.read_u16(), 0, 65535, 0, 255))
    adc_Y = round(Map(VRY.read_u16(), 0, 65535, 0, 255))
    switch = SW.value()

    if adc_X <= 30:
        i = 1
    elif adc_X >= 255:
        i = 2
    elif adc_Y >= 255:
        i = 3
    elif adc_Y <= 30:
        i = 4
    elif switch == 0:
        i = 5

def loop():
    num = 90
    while True:
        direction()
        servo_Angle(num)
        sleep(0.01)

        if i == 1:
            num = 0
        elif i == 2:
            num = 180
        elif i == 3:
            num -= 1
        elif i == 4:
            num += 1

        if num < 0:
            num = 0
        if num > 180:
            num = 180

if __name__ == '__main__':
    loop()