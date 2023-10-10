import serial
import time


# 连接串口
serial = serial.Serial('COM4',115200,timeout=0.5)


if serial.isOpen():
	print ('串口已打开')
else:
	print ('串口未打开')

def build_pressure_command(channel,data):
    add = channel.to_bytes(length=1, byteorder='big', signed=True)
    a = data.to_bytes(length=2, byteorder='big', signed=True)
    begin = b"q"
    ending = b"\r\n"
    command = begin + add + a + ending
    return command

command = build_pressure_command(2,1000)

serial.write(command)  
    # ser.write(cristin)      #串口写数据
print ('You Send Data:', command)
time.sleep(2)


alert = serial.readall()

print ('receive data is :',alert) 
print (len(alert))

#关闭串口
serial.close()
 
if serial.isOpen():
	print ('串口未关闭')
else:
	print ('串口已关闭')