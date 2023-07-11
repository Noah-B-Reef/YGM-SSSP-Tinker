
# import required libraries
import networkx as nx
import numpy as np
import csv
import sys

# read in command line arguements
graph_type = sys.argv[1]
num_nodes = int(sys.argv[2])
seed_type = int(sys.argv[3])
data_path = sys.argv[4]
soln_path = sys.argv[5]

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
    
    G = nx.Graph()
    graph = results
    G.add_weighted_edges_from(graph)

    # sort list before returning
    sol = [(k,v) for k,v in nx.shortest_path_length(G,0,weight="weight").items()]
    results.sort(key=lambda x: x[0])
    return results, sol

# convert graph to csv file
def to_file(data,solution):
    try:
        f = open(data_path, "x")
        g = open(soln_path, "x")
    except:
        f = open(data_path, "w")
        g = open(soln_path, "w")
        
    csv_out = csv.writer(f)
    csv_soln = csv.writer(g)

    csv_out.writerow(["source", "end", "weight"])
    csv_soln.writerow(["node", "dist"])

    for row in data:
        csv_out.writerow(row)

    for row in solution:
        csv_soln.writerow(row)

graph, solutions = graph_gen()
to_file(graph, solutions)
