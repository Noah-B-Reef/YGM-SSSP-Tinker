import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Initialise the subplot function using number of rows and columns
figure, axis = plt.subplots(5, 2, sharey=True)
figure.set_size_inches(18.5, 10.5)

count = 0

for i in [1,2,3,4,5]:
    for j in [5]:
        dfSet = pd.read_csv("/home/noahr/YGM_SSSP_Tinker/examples/SSSP_Directed_Experiements_Set/experiment_" + str(i) + "_" + str(j)+ "_set_buck.csv")
        dfFree = pd.read_csv("/home/noahr/YGM_SSSP_Tinker/examples/SSSP_Undirected_Experiments/experiment_" + str(i) + "_" +str(j)+ "_free_buck.csv")


        num_edges_set = dfSet["# of edges"].to_numpy()
        sssp_runtime_set = dfSet["SSSP time  (ms)"].to_numpy()
        teps_set = np.divide(sssp_runtime_set, num_edges_set)
        scale_set = dfSet["scale"].to_numpy()

        num_edges_free = dfFree["# of edges"].to_numpy()
        sssp_runtime_free = dfFree["SSSP time  (ms)"].to_numpy()
        teps_free = np.divide(sssp_runtime_free, num_edges_free)
        scale_free = dfFree["scale"].to_numpy()


        # For Sine Function
        axis[count, 0].plot(scale_free, sssp_runtime_free)
        axis[count, 0].set_title("Wall Time Free " + str(i*j) + " ranks")
        axis[count,0].set_xlabel("scale")
        axis[count,0].set_ylabel("Wall Time (ms)")

        # For Cosine Function
        axis[count, 1].plot(scale_set, sssp_runtime_set)
        axis[count, 1].set_title("Wall Time Set " + str(i*j) + " ranks")
        axis[count,1].set_xlabel("scale")
        axis[count,1].set_ylabel("Wall Time (ms)")

        count += 1
# using padding
figure.tight_layout(pad=5.0)
# Combine all the operations and display
plt.savefig("wall_time.png")


