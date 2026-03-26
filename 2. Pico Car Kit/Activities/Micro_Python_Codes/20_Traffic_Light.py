from machine import Pin
from time import sleep

red = Pin(2, Pin.OUT)
yellow = Pin(3, Pin.OUT)
green = Pin(4, Pin.OUT)

while True:
    red.value(1)
    yellow.value(0)
    green.value(0)
    sleep(5)

    red.value(0)
    yellow.value(1)
    sleep(2)

    yellow.value(0)
    green.value(1)
    sleep(5)

    green.value(0)