import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import os
head_tag = '"ROB Size,Width,IQ Size,#Instructions,# Cycles,IPC"'

# version = "iqs_gcc"
# lab = "gcc"
# command = "echo " + head_tag + " > ./experiments/" + version + ".csv"
# os.system(command)
# for width in [1,2,4,8]:
#     for iqsize in [8, 16, 32, 64, 128, 256]:
#         command = "./sim " + "512 "+ str(iqsize) +" " + str(width) + " ./val_trace_" + lab + "1 >> ./experiments/" + version + ".csv"
#         os.system(command)
#
# version = "iqs_perl"
# lab = "perl"
# command = "echo " + head_tag + " > ./experiments/" + version + ".csv"
# os.system(command)
# for width in [1,2,4,8]:
#     for iqsize in [8, 16, 32, 64, 128, 256]:
#         command = "./sim " + "512 "+ str(iqsize) +" " + str(width) + " ./val_trace_" + lab + "1 >> ./experiments/" + version + ".csv"
#         os.system(command)


version = "rob_gcc"
lab = "gcc"
command = "echo " + head_tag + " > ./experiments/" + version + ".csv"
os.system(command)

optivals = ["8","16","32","64"]
n=0
for width in [1,2,4,8]:
    for robsize in [32, 64, 128, 256,512]:
        command = "./sim " + str(robsize) + " " + optivals[n] + " " + str(width) + " ./val_trace_" + lab + "1 >> ./experiments/" + version + ".csv"
        os.system(command)
        print(command)
    n = n+1


version = "rob_perl"
lab = "perl"
command = "echo " + head_tag + " > ./experiments/" + version + ".csv"
os.system(command)

optivals = ["8","32","64","128"]
n=0
for width in [1,2,4,8]:
    for robsize in [32, 64, 128, 256,512]:
        command = "./sim " + str(robsize) + " " + optivals[n] + " " + str(width) + " ./val_trace_" + lab + "1 >> ./experiments/" + version + ".csv"
        os.system(command)
        print(command)
    n = n+1