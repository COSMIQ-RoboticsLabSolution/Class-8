from machine import Pin, PWM
import utime

# IR Receiver Pin
PIN = Pin(2, Pin.IN, Pin.PULL_UP)

# Motor Pins
MotorPinA_1A = 10
MotorPinA_1B = 11
MotorPinB_1A = 12
MotorPinB_1B = 13

# Motor Setup
motorA1 = PWM(Pin(MotorPinA_1A))
motorA2 = PWM(Pin(MotorPinA_1B))
motorB1 = PWM(Pin(MotorPinB_1A))
motorB2 = PWM(Pin(MotorPinB_1B))

# Speed
speed = 50000

# Motor Control Function
def motor(A1, A2, B1, B2):
    motorA1.duty_u16(A1)
    motorA2.duty_u16(A2)
    motorB1.duty_u16(B1)
    motorB2.duty_u16(B2)

# Command Execution
N = 0

def exec_cmd(key_val):
    if key_val == 0x18:
        motor(0, speed, 0, speed)  # Forward
    elif key_val == 0x08:
        motor(speed, 0, 0, speed)  # Left
    elif key_val == 0x5A:
        motor(0, speed, speed, 0)  # Right
    elif key_val == 0x52:
        motor(speed, 0, speed, 0)  # Backward
    else:
        motor(0, 0, 0, 0)  # Stop

# Main Program
if __name__ == '__main__':
    while True:
        if PIN.value() == 0:
            count = 0

            # Start signal detection
            while PIN.value() == 0 and count < 200:
                count += 1
                utime.sleep_us(60)

            count = 0
            while PIN.value() == 1 and count < 80:
                count += 1
                utime.sleep_us(60)

            idx = 0
            cnt = 0
            data = [0, 0, 0, 0]

            # Read 32-bit data
            for i in range(32):
                count = 0
                while PIN.value() == 0 and count < 15:
                    count += 1
                    utime.sleep_us(60)

                count = 0
                while PIN.value() == 1 and count < 40:
                    count += 1
                    utime.sleep_us(60)

                if count > 8:
                    data[idx] |= 1 << cnt

                if cnt == 7:
                    cnt = 0
                    idx += 1
                else:
                    cnt += 1

            # Validate and execute command
            if data[0] + data[1] == 0xFF and data[2] + data[3] == 0xFF:
                print("Retrieve key: 0x%02X" % data[2])
                N = data[2]

                if PIN.value() == 1:
                    motor(0, 0, 0, 0)  # Stop
                else:
                    exec_cmd(N)