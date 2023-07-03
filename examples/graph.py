
# import required libraries
import networkx as nx
import numpy as np
import csv
import sys

# read in command line arguements
graph_type = sys.argv[0]
num_nodes = sys.argv[1]
seed_type = sys.argv[2]


# generate a random graph with weights
def graph_gen():
    
    results = []
    
    # generate graph
    if graph_type == "ladder":
        G = nx.ladder_graph(num_nodes)
    elif graph_type == "complete":
        G = nx.complete_graph(num_nodes)
    elif graph_type == "path":
        G = nx.path_graph(num_nodes)
    elif graph_type == "cycle":
        G = nx.cycle_graph(num_nodes)
    else:
        G = nx.ladder_graph(num_nodes)

    # set seed
    np.random.seed(seed_type)

    # generate weights
    for edge in G.edges:
        weight = np.random.randint(1,20)
        results.append((edge[0],edge[1],weight))
        results.append((edge[1], edge[0], weight))
        
    # sort list before returning
    results.sort(key=lambda x: x[0])
    return results, nx.shortest_path_length(G,0,weight="weight")

# convert graph to csv file
def to_file(data,solution):
    try:
        f = open("data.csv", "x")
        g = open("soln.csv", "x")
    except:
        f = open("data.csv", "w")
        g = open("data.csv", "w")
        
    csv_out = csv.writer(f)
    csv_soln = csv.writer(g)

    csv_out.writerow(["source", "end", "weight"])
    csv_soln.writerow(["node", "dist"])

    for row in data:
        csv_out.writerow(row)

    for row in solution:
        csv_soln.writerow(row)

