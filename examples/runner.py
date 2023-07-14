import os
import time

for i in range(1,100):

    #os.system("export mama_path=/home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out")
    #os.system("export mia_data=/home/noahr/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    os.system("srun -o ~/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out ~/YGM_SSSP_Tinker/build/examples/sssp 100 3 ~/YGM_SSSP_Tinker/examples/data/data" + str(i) + ".csv")
    time.sleep(1)
    os.system("python checker.py /home/noahr/YGM_SSSP_Tinker/examples/output/testJob" + str(i) +".out /home/noahr/YGM_SSSP_Tinker/examples/data/soln" + str(i) +".csv")
