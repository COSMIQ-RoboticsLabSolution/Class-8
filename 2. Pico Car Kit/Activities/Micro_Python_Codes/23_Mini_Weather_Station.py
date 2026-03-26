from machine import Pin, I2C
import utime
import dht
from lcd1602 import LCD1602

# Initialize I2C for LCD
i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=400000)
lcd = LCD1602(i2c)

# Initialize DHT11 sensor
dht_sensor = dht.DHT11(Pin(2))

def read_data():
    try:
        dht_sensor.measure()
        temp = dht_sensor.temperature()
        hum = dht_sensor.humidity()
        return temp, hum
    except:
        return None, None

def display_data(temp, hum):
    lcd.clear()
    
    if temp is not None and hum is not None:
        lcd.write(0, 0, "Temp: {} C".format(temp))
        lcd.write(0, 1, "Hum: {} %".format(hum))
    else:
        lcd.write(0, 0, "Sensor Error")
        lcd.write(0, 1, "Check Wiring")

# Main loop
while True:
    temperature, humidity = read_data()
    display_data(temperature, humidity)
    utime.sleep(2)