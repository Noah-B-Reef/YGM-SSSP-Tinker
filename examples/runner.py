import os
import time
import pandas as pd
import csv

results = pd.DataFrame(columns=["scale", "wall time(ms)", "# of edges"])

for i in range(2,20):


    '''
    #os.system("export mama_path=/home/molliep/ygm/examples/output/testJob" + str(i) +".out")
    #os.system("export mia_data=/home/molliep/ygm/examples/data/data" + str(i) + ".csv")
    #os.system("srun -o ~/ygm/examples/output/testJob" + str(i) +".out ~/ygm/build/examples/sssp 100 3 ~/ygm/examples/data/data" + str(i) + ".csv")
    os.system("os.system("srun -o ~/ygm/examples/output/testJob" + str(i) +".out ~/ygm/build/examples/sssp 100 3 ~/ygm/examples/data/data" + str(i) + ".csv")")
    time.sleep(1)
    os.system("python checker.py /home/molliep/ygm/examples/output/testJob" + str(i) +".out /home/molliep/ygm/examples/data/soln" + str(i) +".csv")
    '''
    print("Scale n = " + str(i))
    start = time.time()
    os.system("srun -o exp.out ../build/examples/sssp " + str(i))
    end = time.time()
    print("SSSP Ran in: " + str(end-start))
    time.sleep(3)
    f = open("exp.out", "r")
    results.loc[len(results.index)]  = [i, f.readline().strip(), f.readline().strip()]
    f.close()
    print("added to dataframe")

results.to_csv("experiment_N2.csv", index=False)
