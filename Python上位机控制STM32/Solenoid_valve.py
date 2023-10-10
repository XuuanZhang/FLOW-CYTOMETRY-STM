import serial
import time


# 连接串口
serial = serial.Serial('COM4',115200)


if serial.isOpen():
	print ('串口已打开')
else:
	print ('串口未打开')

data = 'Q'    #发送的数据，单通道赋予数值
turn = '\r\n'
cristin = (data + turn).encode()

serial.write(cristin)    #串口写数据
print ('You Send Data:', cristin)
 	
time.sleep(0.1)
	# while True:
alert = serial.readline()    #串口读数据

print ('receive data is :',alert) 

 
#关闭串口
serial.close()
 
if serial.isOpen():
	print ('串口未关闭')
else:
	print ('串口已关闭')