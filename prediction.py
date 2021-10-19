from tensorflow.keras import models
import numpy as np
import serial

judging_data = 4
features = 43
inf = 10000

model = models.load_model('test_model.h5')

port = 'COM5'
baudrate = 9600

ser = serial.Serial(port, baudrate, timeout=1)

data_list = []

while True:
	try:
		line = ser.readline().decode("utf-8")
		#line = line[:-1]
		exec("temp_dic = {" + line + "}")
		data_list.append(list(temp_dic.values()))
		while len(data_list) > judging_data:
			data_list.pop(0)
		if len(data_list) < judging_data:
			continue
		data = np.array(data_list)
		print(data)
		#data = data.reshape((1, judging_data, features))
		prediction = model.predict(data.reshape(1, judging_data, features))
		pred = np.argmax(prediction)
		print(pred)
		pred = str(pred).encode('utf-8') 
		ser.write(pred)
	except SyntaxError as e0:
		print(e0)
		print("Retry")
		continue
	except ValueError as e1:
		print(e1)
		print("Retry")
		continue