from tensorflow.keras import models
import numpy as np
import serial
from sklearn.preprocessing import MinMaxScaler

### ???? ?? ###
judging_data = 4
features = 43
### ???? ?? ###
inf = 3000

model = models.load_model('CNN2D_model2 (1).h5')

port = 'COM6'
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
		data_imp = data[:, :-2]
		data_inf = data[:, -2:-1]
		data_temp = data[:, -1:]
		scaler = MinMaxScaler()
		scaler.fit(data_imp)
		data_imp = scaler.transform(data_imp)
		scaler.fit(data_inf)
		data_inf = scaler.transform(data_inf)
		scaler.fit(data_temp)
		data_temp = scaler.transform(data_temp)
		data = np.concatenate((data_imp, data_inf, data_temp), axis=1)
		print(data)
		#data = data.reshape((1, judging_data, features))
		prediction = model.predict(data.reshape(1, judging_data, features, 1))
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