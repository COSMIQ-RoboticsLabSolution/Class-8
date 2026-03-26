from machine import Pin, ADC, PWM
from time import sleep

A_1A_pin = 15
Pot_pin = 0

def setup():
    global A_1A, pot_ADC
    A_1A = PWM(Pin(A_1A_pin))
    A_1A.freq(1000)
    pot_ADC = ADC(Pot_pin)

def loop():
    while True:
        value = pot_ADC.read_u16()
        A_1A.duty_u16(value)
        sleep(0.2)

if __name__ == '__main__':
    setup()
    loop()