from machine import Pin, ADC, PWM
from time import sleep

# Flame Sensor (Analog)
flame_sensor = ADC(0)   # GP26 (ADC0)

# Buzzer
buzzer = PWM(Pin(14))

# RGB LED
Led_R = PWM(Pin(11))
Led_G = PWM(Pin(12))
Led_B = PWM(Pin(13))

# Set PWM frequency
Led_R.freq(2000)
Led_G.freq(2000)
Led_B.freq(2000)

# Buzzer ON
def alarm():
    buzzer.duty_u16(2000)
    buzzer.freq(1500)

# Buzzer OFF
def bequiet():
    buzzer.duty_u16(0)

# Main loop
while True:
    flame_value = flame_sensor.read_u16()
    print("Flame Value:", flame_value)

    # Fire detected (LOW value usually means flame detected)
    if flame_value < 20000:
        print("🔥 FIRE DETECTED!")

        # Red LED ON
        Led_R.duty_u16(65535)
        Led_G.duty_u16(0)
        Led_B.duty_u16(0)

        alarm()

    # No fire
    else:
        print("Safe")

        # Green LED ON
        Led_R.duty_u16(0)
        Led_G.duty_u16(65535)
        Led_B.duty_u16(0)

        bequiet()

    sleep(0.3)