import serial
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

serial = serial.Serial('COM4',256000,timeout=0.5)

if serial.isOpen():
	print ('串口打开')

f = open('./test.txt','w',encoding='iso8859-1')

try:
    getBytes=b''
    num_count = 0
    detection_num = 0
    data = 'H'    #发送的数据，单通道赋予数值
    turn = '\r\n'
    cristin = (data + turn).encode()
    serial.write(cristin) 

    current_time = datetime.now()
    print("The current time is", current_time)

    while True:
        if serial.inWaiting()>0:
            b1 = serial.read(1)                # read the first byte of data
            b2 = serial.read(1)                # read the second byte of data
            data = b1 + b2                     # 将两个byte组合在一起
            # print(data)

            data_int = int.from_bytes(data, byteorder='big')
            # print(data_int)

        # if data != getBytes:

            f.write(str(data_int))

            f.write('\n')
            num_count+=1
            # print(num_count)
        
        if num_count >=20000:

            f.close()
            break
        
    print("over")
    current_time = datetime.now()
    print("The current time is", current_time)

    data = serial.readline()
    print("detection_num is:" ,data.decode())


                
except KeyboardInterrupt:
    print('off')
    if serial != None:
        f.close()
        serial.close()

f.close()
serial.close()

a = np.loadtxt('test.txt',  dtype=int) 
#skiprows为跳过指定行  
plt.title("Wave detection plot")
plt.plot(a,linestyle=':',color='r')
plt.show()