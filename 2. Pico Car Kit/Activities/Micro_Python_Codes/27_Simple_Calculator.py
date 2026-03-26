from machine import Pin, I2C
import utime
from ssd1306 import SSD1306_I2C

debug = True

# ---------------- OLED ----------------
i2c = I2C(0, scl=Pin(21), sda=Pin(20), freq=400000)
oled = SSD1306_I2C(128, 64, i2c)

# ---------------- KEYPAD ----------------
keyName = [
    ['1','2','3','+'],
    ['4','5','6','-'],
    ['7','8','9','*'],
    ['c','0','=','/']
]

keypadRowPins = [13,12,11,10]
keypadColPins = [9,8,7,6]

row = []
col = []
keypadState = []

for i in keypadRowPins:
    row.append(Pin(i, Pin.IN, Pin.PULL_UP))
    keypadState.append([0,0,0,0])

for i in keypadColPins:
    col.append(Pin(i, Pin.OUT))

# ---------------- CALCULATION ----------------
def solve(oprt, oprdA, oprdB):
    if oprt == "+":
        return oprdA + oprdB
    elif oprt == "-":
        return oprdA - oprdB
    elif oprt == "*":
        return oprdA * oprdB
    elif oprt == "/":
        return round(oprdA / oprdB, 6)

def calc(lst):
    operand = []
    operator = []

    for i in lst:
        if debug:
            print(i)

        if i in ['+','-']:
            while len(operator) != 0:
                b = operand.pop()
                a = operand.pop()
                c = operator.pop()
                operand.append(solve(c,a,b))
            operator.append(i)

        elif i in ['*','/']:
            while len(operator) != 0 and operator[-1] in ['*','/']:
                b = operand.pop()
                a = operand.pop()
                c = operator.pop()
                operand.append(solve(c,a,b))
            operator.append(i)

        else:
            operand.append(i)

    while len(operator) != 0:
        b = operand.pop()
        a = operand.pop()
        c = operator.pop()
        operand.append(solve(c,a,b))

    return operand[0]

# ---------------- KEYPAD READ ----------------
def keypadRead():
    j_ifPressed = -1
    i_ifPressed = -1

    for i in range(len(col)):
        col[i].low()
        utime.sleep(0.005)

        for j in range(len(row)):
            pressed = not row[j].value()

            if pressed and keypadState[j][i] != pressed:
                keypadState[j][i] = pressed

            elif not pressed and keypadState[j][i] != pressed:
                keypadState[j][i] = pressed
                j_ifPressed = j
                i_ifPressed = i

        col[i].high()

    if j_ifPressed != -1:
        return keyName[j_ifPressed][i_ifPressed]
    return -1

# ---------------- OLED PRINT ----------------
def printOled(lst):
    oled.fill(0)

    string = ""
    for i in lst:
        string += str(i)

    y = 0
    for i in range(0, len(string), 16):
        oled.text(string[i:i+16], 0, y)
        y += 10

    oled.show()

# ---------------- MAIN ----------------
inputList = ['']

oled.fill(0)
oled.text("Elecrow", 30, 10)
oled.text("Calculator", 20, 30)
oled.show()
utime.sleep(2)

while True:
    key = keypadRead()

    if key != -1:
        print("Key:", key)

        # Numbers
        if key.isdigit() or key == '.':
            inputList[-1] += key

        # Operators
        elif key in ['+','-','*','/']:
            if inputList[-1] != '':
                inputList[-1] = float(inputList[-1])
                inputList.append(key)
                inputList.append('')

        # Clear
        elif key == 'c':
            inputList = ['']

        # Equals
        elif key == '=':
            if inputList[-1] != '':
                inputList[-1] = float(inputList[-1])

            try:
                ans = calc(inputList)
                inputList = [str(ans)]
            except:
                inputList = ["Error"]

        printOled(inputList)

    utime.sleep(0.1)