from machine import Pin, ADC, PWM
from time import sleep

Soil_moisture_pin = 0
buzzer = PWM(Pin(14))

Led_R = PWM(Pin(11))
Led_G = PWM(Pin(12))
Led_B = PWM(Pin(13))

Led_R.freq(2000)
Led_G.freq(2000)
Led_B.freq(2000)

def setup():
    global Moisture
    Moisture = ADC(Soil_moisture_pin)

def playtone(freq):
    buzzer.duty_u16(1000)
    buzzer.freq(freq)

def bequiet():
    buzzer.duty_u16(0)

def check_moisture(x):
    if x > 20000:
        playtone(330)
        Led_R.duty_u16(65535)
        Led_G.duty_u16(0)
        Led_B.duty_u16(0)
    elif 15000 < x < 20000:
        bequiet()
        Led_R.duty_u16(0)
        Led_G.duty_u16(65535)
        Led_B.duty_u16(0)

def loop():
    while True:
        Moist = Moisture.read_u16()
        print("Moisture:", Moist)
        check_moisture(Moist)
        sleep(0.2)

if __name__ == '__main__':
    setup()
    loop()