# -*- coding: utf-8 -*-
"""
Created on Mon Aug 14 10:01:22 2017


@author: Tanry
"""
import serial
import cv2
import matplotlib as plt
import binascii
import os
from PIL import Image
import numpy as np
from numpy import linalg
import matplotlib.pyplot as plt
import math  
import string  
import os 




try:
    ser=serial.Serial( #下面这些参数根据情况修改
                  port='COM3',
                  baudrate=256000,
                  parity=serial.PARITY_NONE,
                  stopbits=serial.STOPBITS_ONE,
                  bytesize=serial.EIGHTBITS)
    print ("COM open Success!!")
    ComSus=1
except Exception as e:
    print(e) 
    
    print ("COM open Fail!!")
    ComSus=0
try:
    os.remove("123.txt")
except Exception as e:
    print(e) 
data=[]
starts=[0,0,0,0]
success=0
txt=open("123.txt","a")
if (ComSus):
    i=10000
    while (i):
        i=i-1
        temp =ser.read()
        starts[3]=starts[2]
        starts[2]=starts[1]
        starts[1]=starts[0]
        starts[0]=temp
        # print starts
        if (starts==["t","r","t","s"]):
            print ("start collect")
            while(1):
                temp =ser.read()
                starts[3]=starts[2]
                starts[2]=starts[1]
                starts[1]=starts[0]
                starts[0]=temp
                if (starts==["r","e","v","o"]):
                    print ("collect success")
                    i=0
                    success=1
                    break
                else:
                    txt.write(temp)
        
    if (success==0):
        print ("no image!!") 
    else:
        print ("success!!")
    ser.close()
txt.close()
print ("COM close!!")
