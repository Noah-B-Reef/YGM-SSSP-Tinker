import os
import time
import pandas as pd
import csv

df = pd.DataFrame(columns=["scale", "wall time(ms)", "# of edges"])

for i in range(2,100):


    '''
    #os.system("export mama_path=/home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out")
    #os.system("export mia_data=/home/noahr/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    #os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    os.system("os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")")
    time.sleep(1)
    os.system("python checker.py /home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out /home/noahr/YGM_SSSP_Tinker/examples/data/soln" + str(i) +".csv")
    '''

    os.system("srun -o my_graph.csv ../build/examples/rmat_gen_graphs " + str(i))
    print("csv made!")
    time.sleep(1)


     # open file
    with open("my_graph.csv", "r") as csvfile:
        data=[tuple(line) for line in csv.reader(csvfile)]
    data = data[1::]
    data.sort()

    df = pd.DataFrame(data, columns=["source", "end", "weight"])
    df.to_csv("my_graph.csv")

    os.system("srun -o exp.out ../build/examples/sssp my_graph.csv")
    print("sssp ran!")
    time.sleep(1)
    f = open("exp.out", "r")
    df.insert(i, f.readline(), f.readline())
    f.close()
    print("added to dataframe")

df.to_csv("experiment.csv")