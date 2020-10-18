import serial

class DoorController():
    def __init__(self, port='/dev/ttyUSB0', baud=19200):
        print("[+] init door controller")
        self.ser = serial.Serial(port, baud)

    def setLed(self, on):
        cmd = b'L' + (b'1' if on else b'0')
        self.ser.write(cmd)

    def dispenseMask(self):
        cmd = b'M'
        self.ser.write(cmd)

    def sanitize(self):
        cmd = b'S'
        self.ser.write(cmd)

    def reply_sanitize(self):
        return self.ser.read()

    def check_occupancy(self):
        return self.ser.read()
