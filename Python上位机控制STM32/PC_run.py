import serial
import time
serial = serial.Serial('COM3',115200,timeout=0.5)
if serial.isOpen():
	print ('串口已打开')
else:
	print ('串口未打开')

# MCP4728单通道控制
def build_pressure_single_command(channel,data):
    add = channel.to_bytes(length=1, byteorder='big', signed=True)
    a = data.to_bytes(length=2, byteorder='big', signed=True)
    begin = b"Q"
    ending = b"\r\n"
    command = begin + add + a + ending
    return command

# MCP4728全通道控制
def build_pressure_all_command(data):
    a = data.to_bytes(length=2, byteorder='big', signed=True)
    begin = b'P'
    ending = b"\r\n"
    command = begin + a + ending
    return command

# GPIO控制电磁阀，state中1代表开启，0代表关闭
def bulid_valve_command(channel,state):
    channel = channel.to_bytes(length=1, byteorder='big', signed=True)
    state = state.to_bytes(length=1, byteorder='big', signed=True)
    begin = b"V"
    ending = b"\r\n"
    command = begin + channel + state + ending
    return command

# 单通道outputC赋值1000
command = build_pressure_single_command(2,1000)
# 全通道赋值1000
command = build_pressure_all_command(1000)
# 控制1号电磁阀开启
command = bulid_valve_command(1,1)

serial.write(command)  
#     # ser.write(cristin)      #串口写数据
# print ('You Send Data:', command)
time.sleep(0.1)

alert = serial.readline()
print ('receive data is :',alert)



serial.close()
 
if serial.isOpen():
	print ('串口未关闭')
else:
	print ('串口已关闭')