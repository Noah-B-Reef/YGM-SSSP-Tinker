import os
import time
import pandas as pd
import csv
import numpy as np
import sys

COMPUTE_NODES = 16
rmat_scale = 20
kruskal_ranks = 2
upper_tpn = 8

# work up to 16-32 compute nodes
# 8 tpn
results = pd.DataFrame(columns=["scale", "SSSP time (ms)", "# of edges"])

for i in range(2, upper_tpn + 1):
    print("*** collecting data for " + str(i) + " tpn ***")
    start = time.time()
    os.system("srun -N " + str(COMPUTE_NODES) + " --tasks-per-node=" + str(i) + " -o exp.out ../build/examples/sssp " + str(rmat_scale))
    end = time.time()
    print("time to generate graph and run algorithm: " + str(end-start))
    time.sleep(5)
    f = open("exp.out", "r")
    results.loc[len(results.index)] = [i, f.readline().strip(), f.readline().strip()]
    f.close()
    print("added to dataframe")

results.to_csv("weak_scaling/ws_N" + str(COMPUTE_NODES) + "_" + sys.argv[1] + ".csv", index=False)
