import csv
import os
import sys
import pandas as pd

def out_to_dict(path):
    result = {}
    f = open(path, "r")
    for line in f:
       info = line.split(", ")
       key = int(info[0])
       val = float(info[1])
       result[key] = val
    return result


# test
path = sys.argv[1]
sol = sys.argv[2]

my_Dict = out_to_dict(path)

df = pd.read_csv(sol)
soln = dict(zip(df.node, df.dist))

file = path.split('/')

if(my_Dict == soln):
    print(file[-1] + " passed!")
else:
    print(file[-1] + " failed!")


