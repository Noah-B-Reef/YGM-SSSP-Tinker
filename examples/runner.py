import os
import time
import pandas as pd
import csv

df = pd.DataFrame(columns=["scale", "wall time(ms)", "# of edges"])

for i in range(15,100):


    '''
    #os.system("export mama_path=/home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out")
    #os.system("export mia_data=/home/noahr/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    #os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    os.system("os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")")
    time.sleep(1)
    os.system("python checker.py /home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out /home/noahr/YGM_SSSP_Tinker/examples/data/soln" + str(i) +".csv")
    '''

    os.system("srun -o Experiment/my_graph" + str(i) + ".csv ../build/examples/rmat_gen_graphs " + str(i))
    print("csv made!")
    time.sleep(3)


     # open file
    with open("Experiment/my_graph" + str(i) + ".csv", "r") as csvfile:
        data=[tuple(line) for line in csv.reader(csvfile)]
    data = data[1::]
    data.sort()
    df = pd.DataFrame(data, columns=["source", "end", "weight"])
    df.to_csv("Experiment/my_graph" + str(i) + ".csv", index=False)

    time.sleep(3)
    os.system("srun -o exp.out ../build/examples/sssp Experiment/my_graph" + str(i) + ".csv")
    print("sssp ran!")
    time.sleep(3)
    f = open("exp.out", "r")
    df.loc[len(df.index)]  = [i, f.readline(), f.readline()]
    f.close()
    print("added to dataframe")

df.to_csv("experiment.csv")
