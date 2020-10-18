import serial

def DoorController():
    
    def __init__(self, port='/dev/ttyUSB0', baud=19200): 
        self.ser = serial.Serial(port, baud)
    
    def setLed(on):
        cmd = b'L' + (b'1' if on else b'0')  
        ser.write(cmd)

    def dispenseMask():
        cmd = b'M'
        self.ser.write(cmd)


