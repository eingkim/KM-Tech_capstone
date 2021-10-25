import serial

port = 'COM6'
baudrate = 9600

ser = serial.Serial(port, baudrate, timeout=1)
print('Arduino Serial START')

while True:
	line = ser.readline().decode("utf-8")
	if len(line)>0:
		line = line + ","
		print(line)
		f = open("10.23_exp3.txt", 'a')
		f.write(line)
		f.close()