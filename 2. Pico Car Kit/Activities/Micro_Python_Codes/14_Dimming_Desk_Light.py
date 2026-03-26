from machine import Pin, ADC, PWM
from time import sleep

Led_pin = 15
Potentiometer_pin = 0

def setup():
    global LED, Pot_ADC
    LED = PWM(Pin(Led_pin))
    LED.freq(2000)
    Pot_ADC = ADC(Potentiometer_pin)

def loop():
    while True:
        value = Pot_ADC.read_u16()
        LED.duty_u16(value)
        sleep(0.2)

if __name__ == '__main__':
    setup()
    loop()