from machine import Pin, SPI
import mfrc522

sck = 6
mosi = 7
miso = 4
cs = 5
rst = 22

rdr = mfrc522.MFRC522(sck=sck, mosi=mosi, miso=miso, rst=rst, cs=cs)

print("Scan RFID card...")

while True:
    (stat, tag_type) = rdr.request(rdr.REQIDL)

    if stat == rdr.OK:
        (stat, uid) = rdr.anticoll()
        if stat == rdr.OK:
            print("Card UID:", uid)