import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime


a = np.loadtxt('wave detection.txt',  dtype=int) 
#skiprows为跳过指定行  
plt.title("Wave detection plot")
plt.plot(a,linestyle=':',color='r')
plt.show()