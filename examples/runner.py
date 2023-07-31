import os
import time
import pandas as pd
import csv
import numpy as np

    '''
    #os.system("export mama_path=/home/molliep/ygm/examples/output/testJob" + str(i) +".out")
    #os.system("export mia_data=/home/molliep/ygm/examples/data/data" + str(i) + ".csv")
    #os.system("srun -o ~/ygm/examples/output/testJob" + str(i) +".out ~/ygm/build/examples/sssp 100 3 ~/ygm/examples/data/data" + str(i) + ".csv")
    os.system("os.system("srun -o ~/ygm/examples/output/testJob" + str(i) +".out ~/ygm/build/examples/sssp 100 3 ~/ygm/examples/data/data" + str(i) + ".csv")")
    time.sleep(1)
    os.system("python checker.py /home/molliep/ygm/examples/output/testJob" + str(i) +".out /home/molliep/ygm/examples/data/soln" + str(i) +".csv")
    '''
for i in range(2,25):
    print("Scale n = " + str(i))
    start = time.time()
    os.system("srun -o exp.out ../build/examples/bf_sssp " + str(i))
    end = time.time()
    print("SSSP Ran in: " + str(end-start))
    time.sleep(3)
    f = open("exp.out", "r")
    results.loc[len(results.index)]  = [i, f.readline().strip(), f.readline().strip()]
    f.close()
    print("added to dataframe")

results.to_csv("bf_experiment_dir_N6_tpn32.csv", index=False)
