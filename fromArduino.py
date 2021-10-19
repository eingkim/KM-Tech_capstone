import serial

port = 'COM5'
baudrate = 9600

ser = serial.Serial(port, baudrate, timeout=1)
print('Arduino Serial START')

while True:
	line = ser.readline().decode("utf-8")
	print(line)
	f = open("tempResult.txt", 'a')
	f.write(line)
	f.close()