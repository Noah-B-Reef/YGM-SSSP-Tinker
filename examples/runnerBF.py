import os
import time
import pandas as pd
import csv
import numpy as np
from tabulate import tabulate


baseLineN = 4
baseLinen = 1
for i in range(20,31):
        results = pd.DataFrame(columns=["scale", "BF time  (ms)", "# of edges"])
        print("Scale n = " + str(i))
        start = time.time()
        os.system("srun -N " + str(baseLineN) + " --ntasks-per-node=" + str(baseLinen) + " -o expB.out ../build/examples/bf_sssp " + str(i))
        end = time.time()
        print("SSSP Ran in: " + str(end-start))
        time.sleep(5)
        f = open("expB.out", "r")

        sssp_time = f.readline().strip()
        num_edges = f.readline().strip()

        results.loc[len(results.index)]  = [i, sssp_time, num_edges]
        f.close()
        print("added to dataframe")
        print(tabulate(results, headers = 'keys', tablefmt = 'psql'))

        results.to_csv("Strong_Scaling_Studies/experiment_" + str(baseLineN) + "_" + str(baseLinen) + "_bf.csv", index=False)

        if (baseLinen == 8):
                baseLinen = 1
                baseLineN *= 2
        else:
                baseLinen *= 2
        