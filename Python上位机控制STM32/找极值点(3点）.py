from scipy.misc import electrocardiogram
import matplotlib.pyplot as plt
# import scipy.signal
import numpy as np
import pandas as pd


y = []
a = []

def excel_one_line_to_list():
    df = pd.read_excel(r"C:\Users\77521\Desktop\scope_3.xlsx", usecols=[1], names=None)  # 读取项目名称列,不要列名
    df_li = df.values.tolist()
    for s_li in df_li:
        y.append(s_li[0])
    # print(result)


excel_one_line_to_list()


# x = electrocardiogram()[2000:4000]
# 建立一个数据组测试，如果是已有数据，考虑如何导入后形成numpy的数组
# y = np.array([1,1,1,2,3,4,2,1,3,4,7,6,3,2,1,3,2,1,9,7,4,2,0,8,8,8,7,1,1,1,6,2,9,9,9])
# 此处建立一个list而不是array，因为只有list是可以空数据建立，不定长度，这个list是为了储存找到的极值的索引
# a = []


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
    x = np.zeros(3)  # 初始化一个3个元素的数组，并赋值0
    x[0:3] = y[0:3]  # 将x数组的前3个数据变成array的前3个数据
    for i in range(3, len(y)):  # 前面数组的初始化已有3个数据了，所以从3开始了
        shift(x, y[i])
        # print(x)
        if x[1] >= x[0] and x[1] >= x[2] and x[1] > 7:
            print(x)
            a.append(i-1)
            print(i)
    print(a)



find_peak(y)  # 找极点后，数据的索引存储在a里面
peaks = np.array(a)  # 将list转变成array，否则后面不好使用
# print(peaks)
# print(type(peaks))
plt.plot(y)
plt.plot(peaks, np.array(y)[peaks], "o")
# plt.title("寻找极值")
plt.show()
