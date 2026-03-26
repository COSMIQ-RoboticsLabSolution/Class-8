from machine import Pin
import utime

# Simple placeholder clock
while True:
    print("Time:", utime.localtime())
    utime.sleep(1)