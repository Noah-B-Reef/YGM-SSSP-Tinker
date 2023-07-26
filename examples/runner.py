import os
import time
import pandas as pd
import csv

results = pd.DataFrame(columns=["scale", "wall time(ms)", "# of edges"])

for i in range(2,30):


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
    os.system("srun -o exp.out ../build/examples/sssp " + str(i))
    end = time.time()
    print("SSSP Ran in: " + str(end-start))
    time.sleep(3)
    f = open("exp.out", "r")
    results.loc[len(results.index)]  = [i, f.readline().strip(), f.readline().strip()]
    f.close()
    print("added to dataframe")

results.to_csv("experiment.csv")
