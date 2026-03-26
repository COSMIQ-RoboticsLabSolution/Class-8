from machine import Pin, I2C
from ssd1306 import SSD1306_I2C
from dht import DHT11
import time

# -------------------- SETUP --------------------
# I2C for OLED
i2c = I2C(0, sda=Pin(0), scl=Pin(1), freq=400000)
oled = SSD1306_I2C(128, 64, i2c)

# DHT11 Sensor
dht = DHT11(Pin(2))

# Initial Time: [Year, Month, Day, Hour, Minute, Second]
current_time = [2026, 3, 26, 10, 0, 0]

# -------------------- FUNCTIONS --------------------
def format_time():
    h, m, s = current_time[3], current_time[4], current_time[5]
    return "Time: {:02d}:{:02d}:{:02d}".format(h, m, s)

def format_date():
    y, mo, d = current_time[0], current_time[1], current_time[2]
    return "Date: {:04d}-{:02d}-{:02d}".format(y, mo, d)

def is_leap(year):
    return (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0)

def update_time():
    current_time[5] += 1  # seconds

    if current_time[5] >= 60:
        current_time[5] = 0
        current_time[4] += 1  # minutes

    if current_time[4] >= 60:
        current_time[4] = 0
        current_time[3] += 1  # hours

    if current_time[3] >= 24:
        current_time[3] = 0
        current_time[2] += 1  # day

    # Days in month
    month = current_time[1]
    year = current_time[0]

    if month in [1,3,5,7,8,10,12]:
        max_day = 31
    elif month in [4,6,9,11]:
        max_day = 30
    elif month == 2:
        max_day = 29 if is_leap(year) else 28

    if current_time[2] > max_day:
        current_time[2] = 1
        current_time[1] += 1

    if current_time[1] > 12:
        current_time[1] = 1
        current_time[0] += 1

# -------------------- MAIN LOOP --------------------
while True:
    oled.fill(0)

    # Read Temperature
    try:
        temp = dht.temperature
    except:
        temp = "Err"

    # Display
    oled.text(format_date(), 0, 0)
    oled.text(format_time(), 0, 12)
    oled.text("Temp: {} C".format(temp), 0, 28)

    oled.show()

    update_time()
    time.sleep(1)