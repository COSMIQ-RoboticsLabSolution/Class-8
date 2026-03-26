from machine import Pin
from time import sleep

leds = [Pin(i, Pin.OUT) for i in range(2, 8)]

while True:
    for led in leds:
        led.value(1)
        sleep(1)

    for led in leds:
        led.value(0)
        sleep(1)