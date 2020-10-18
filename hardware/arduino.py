import serial
dev = serial.Serial('COM4', 9600)
data = []

for _ in range(10):
    dev.write(b'2')
    line = dev.readline()
    data.append(line)

print(data)