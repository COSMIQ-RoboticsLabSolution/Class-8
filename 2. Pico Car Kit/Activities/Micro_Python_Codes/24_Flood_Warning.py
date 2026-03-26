from machine import Pin, ADC, PWM
from time import sleep

# Rain / Water Sensor (Analog)
water_sensor = ADC(0)   # GP26 (ADC0)

# Buzzer
buzzer = PWM(Pin(14))

# RGB LED Pins
Led_R = PWM(Pin(11))
Led_G = PWM(Pin(12))
Led_B = PWM(Pin(13))

# Set PWM frequency
Led_R.freq(2000)
Led_G.freq(2000)
Led_B.freq(2000)

# Function to turn OFF buzzer
def bequiet():
    buzzer.duty_u16(0)

# Function to turn ON buzzer
def alarm():
    buzzer.duty_u16(1000)
    buzzer.freq(1000)

# Main Loop
while True:
    water_value = water_sensor.read_u16()
    print("Water Level:", water_value)

    # Condition: HIGH water level (Flood)
    if water_value > 30000:
        print("⚠️ Flood Warning!")

        # Red LED ON
        Led_R.duty_u16(65535)
        Led_G.duty_u16(0)
        Led_B.duty_u16(0)

        alarm()

    # Condition: Medium water level
    elif 15000 < water_value <= 30000:
        print("Water Level Medium")

        # Yellow (Red + Green)
        Led_R.duty_u16(65535)
        Led_G.duty_u16(65535)
        Led_B.duty_u16(0)

        bequiet()

    # Condition: LOW water level (Safe)
    else:
        print("Water Level Safe")

        # Green LED ON
        Led_R.duty_u16(0)
        Led_G.duty_u16(65535)
        Led_B.duty_u16(0)

        bequiet()

    sleep(0.5)