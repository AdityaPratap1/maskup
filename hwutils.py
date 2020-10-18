import serial

class DoorController():
    def __init__(self, port='/dev/cu.usbmodem141101', baud=9600):
        self.ser = serial.Serial(port, baud)

    def setLed(self, on):
        print('cmd: LED', on); 
        cmd = b'L' if on else b'l'
        self.ser.write(cmd)

    def dispenseMask(self):
        print('cmd: MASK')
        cmd = b'M'
        self.ser.write(cmd)

    def sanitize(self):
        print('cmd: SANITIZE')
        cmd = b'S'
        self.ser.write(cmd)

    def reply_sanitize(self):
        print('cmd: rSANITIZE')
        return self.ser.read()

    def check_occupancy(self):
        print('cmd: OCCUPANCY')
        self.ser.write(b'O') # send request
        return int( self.ser.read() ) 

