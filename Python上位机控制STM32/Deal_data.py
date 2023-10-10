from scipy.misc import electrocardiogram
import matplotlib.pyplot as plt
# import scipy.signal
import numpy as np
import pandas as pd

result=[]
a=[]

with open('test.txt','r') as f:
	for line in f:
		result.append(list(line.strip('\n').split(',')))

result = list(np.array(result).flatten())  #删除数组中的[]
result = str(result).replace("'","")   #删除数组中的'
# 注意，此时result数组变为了str形式的字符串


list_data = result.replace("[","").replace("]","").split(", ")

list_data = [int(d) for d in list_data]
# 重新将result变为数组list_data

# print(list_data)

# print(list_data[20])


# 建立数组，并可以更新左移，此处实验是为了不断取3个数据进行分析
def shift(arr, fill_value):
    a = arr[1]
    b = arr[2]
    arr[0] = a
    arr[1] = b
    # print(arr[0])
    arr[2] = fill_value


# 从数组中寻找峰值，并储存其索引值
def find_peak(arr):
    count = 0
    x = np.zeros(3)  # 初始化一个3个元素的数组，并赋值0
    x[0:3] = list_data[0:3]  # 将x数组的前3个数据变成array的前3个数据
    for i in range(3, len(list_data)):  # 前面数组的初始化已有3个数据了，所以从3开始了
        shift(x, list_data[i])
        # print(x)
        if x[1] >= x[0] and x[1] >= x[2] and x[1] > 7:
            print(x)
            a.append(i-1)
            count = count + 1
    print(a)
    print(count)



find_peak(list_data)  # 找极点后，数据的索引存储在a里面
peaks = np.array(a)  # 将list转变成array，否则后面不好使用
# print(peaks)
# print(type(peaks))
plt.plot(list_data)
plt.plot(peaks, np.array(list_data)[peaks], "o")
# plt.title("寻找极值")
plt.show()