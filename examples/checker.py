import csv
import os

def out_to_dict(path):
    result = {}
    f = open(path, "r")
    for line in f:
       print(line)
       info = line.split(", ")
       key = int(info[0])
       val = float(info[1])
       result[key] = val
    return result
# test
my_Dict = out_to_dict("/home/noahr/YGM_SSSP_Tinker/build/examples/testJob.out")
print(my_Dict)