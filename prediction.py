from tensorflow.keras import models
import numpy as np
import serial
from sklearn.preprocessing import MinMaxScaler

### ???? ?? ###
judging_data = 10
features = 43
### ???? ?? ###
inf = 5000

model = models.load_model('CNN2D_model5.h5')

port = 'COM6'
baudrate = 9600

ser = serial.Serial(port, baudrate, timeout=1)

data_list = []

while True:
	try:
		line = ser.readline().decode("utf-8")
		if len(line) > 0:
			print("len(line):{}".format(len(line)))
			print("line:{}".format(line))
			exec("temp_arr = np.array([" + line + "])")
			if len(temp_arr) != features:
				print(len(temp_arr))
				continue
			arr_imp = temp_arr[:-2]
			arr_inf = temp_arr[-2:-1]
			arr_tmp = temp_arr[-1:]
			scaler = MinMaxScaler()
			scaler.fit(np.array([170.0, inf]).reshape(-1,1))
			arr_imp = scaler.transform(arr_imp.reshape(-1,1))
			scaler.fit(np.array([0, 6.0]).reshape(-1, 1))
			arr_inf = scaler.transform(arr_inf.reshape(-1,1))
			scaler.fit(np.array([23.2187, 28.6562]).reshape(-1,1))
			arr_tmp = scaler.transform(arr_tmp.reshape(-1,1))
			tmp_list = np.concatenate((arr_imp.reshape(1, features-2), arr_inf.reshape(1, 1), arr_tmp.reshape(1,1)), axis=1)
			data_list.append(tmp_list)
			print(data_list)
			while len(data_list) > judging_data:
				data_list.pop(0)
			if len(data_list) < judging_data:
				continue
			data = np.array(data_list).reshape(judging_data, features)
			print(data)
			#data = data.reshape((1, judging_data, features))
			prediction = model.predict(data.reshape(1, judging_data, features, 1))
			pred = np.rint(prediction[0])
			print("prediction:{}".format(prediction))
			#pred[0] += 1
			print("pred:{}".format(pred[0]))
			pred = str(pred[0]).encode('utf-8') 
			ser.write(pred)
	except SyntaxError as e0:
		print(e0)
		print("Retry")
		continue
	except ValueError as e1:
		print(e1)
		print("Retry")
		continue