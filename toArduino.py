import serial
import time

port = 'COM5'
baudrate = 9600

one = 1
two = 0
one = str(one)
two = str(two)

ser = serial.Serial(port, baudrate)

while True:
	if ser.readable():
		val = one.encode('utf-8')
		ser.write(val)
		print(one)
		time.sleep(1)
		val = two.encode('utf-8')
		ser.write(val)
		print(two)
		time.sleep(1)
 