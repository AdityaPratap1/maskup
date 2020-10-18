import serial
import time 

dev = serial.Serial('/dev/cu.usbmodem141101', 9600)
data = []
for i in range(10):
    dev.write(b'L'); 
    time.sleep(0.2)
    dev.write(b'O'); 
    time.sleep(0.2)

