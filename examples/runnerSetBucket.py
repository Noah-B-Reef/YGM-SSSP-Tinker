import os
import time
import pandas as pd
import csv
import numpy as np

for k in range(5,9):
    for j in range(1,11):
            results = pd.DataFrame(columns=["scale", "# of Buckets", "RMAT Gen Time (ms)" , "SSSP time  (ms)", "# of edges"])
            for i in range(2,21):

                    '''
                    #os.system("export mama_path=/home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out")
                    #os.system("export mia_data=/home/noahr/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
                    #os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
                    os.system("os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")")
                    time.sleep(1)
                    os.system("python checker.py /home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out /home/noahr/YGM_SSSP_Tinker/examples/data/soln" + str(i) +".csv")
                    '''
                    print("Scale n = " + str(i))
                    start = time.time()
                    os.system("srun -N " + str(k) + " --ntasks-per-node=" + str(j) + " -o expS.out ../build/examples/sssp " + str(i) + " " + str(1000) + " " + str(3))
                    end = time.time()
                    print("SSSP Ran in: " + str(end-start))
                    time.sleep(5)
                    f = open("expS.out", "r")

                    num_buckets = f.readline().strip()
                    rmat_time = f.readline().strip()
                    sssp_time = f.readline().strip()
                    num_edges = f.readline().strip()

                    results.loc[len(results.index)]  = [i, num_buckets,rmat_time,sssp_time, num_edges]
                    f.close()
                    print("added to dataframe")
                    
            results.to_csv("/home/noahr/YGM_SSSP_Tinker/examples/SSSP_Directed_Experiements_Set/experiment_" + str(k) + "_" + str(j) + "_set_buck.csv", index=False)
