import serial

class DoorController():
    def __init__(self, port=2, baud=9600):
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
        # cmd = b'O'
        # self.ser.write(cmd)
        a = "L"
        data = []
        self.ser.write(a.encode())
        init = str(self.ser.readline())
        data.append(init)
        while self.ser is not None:

            data = str(self.ser.readline())
            print(data)

        return data
