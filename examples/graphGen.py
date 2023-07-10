import os
import numpy as np

for i in range(10):

    graph_choice = np.random.randint(1,4)

    # generate graph
    match graph_choice:
        case 1:
            graph_type = "ladder"
        case 2:
            graph_type = "complete"
        case 3:
            graph_type = "cycle"
        case 4:
            graph_type = "path"
        case _:
            graph_type = "ladder"
        
    
    num_nodes = str(np.random.randint(5,20))
    seed_type = str(np.random.randint(0,5))
    data_path = "data/data" + str(i + 1) + ".csv"
    soln_path = "data/soln" + str(i + 1) + ".csv"

    os.system("python graph.py " + graph_type + " " + num_nodes + " " + seed_type + " " + data_path + " " + soln_path)