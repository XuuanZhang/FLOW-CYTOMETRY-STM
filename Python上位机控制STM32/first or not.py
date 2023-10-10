import serial
import time


# 连接串口
serial = serial.Serial('COM3',9600,timeout=0.5)


if serial.isOpen():
	print ('串口已打开')
else:
	print ('串口未打开')

