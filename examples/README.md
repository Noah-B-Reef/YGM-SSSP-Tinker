# Delta Step SSSP Directions
## Running Algorithms on One Graph
### Running Delta Step SSSP on the Paper Heuristic
- from this directory (ygm/examples), run **cd ../build** and then **make**
- salloc the desired number of nodes, and then **srun examples/delta_\_sssp n"** where n is the rmat scale that you want to have -> 2^n vertices will be produced

### Running Delta Step SSSP on a Fixed Bucket Size
- from this directory (ygm/examples), run **cd ../build** and then **make**
- salloc the desired number of nodes, and then run **srun examples/delta_\_sssp n b d** where n is the rmat scale, b is the number of buckets, and d is delta

### Running Bellman Ford 
- from this directory (ygm/examples), run **cd ../build** and then **make**
- salloc the desired number of nodes, and then run **srun examples/bf_\_sssp n** where n is the rmat scale

## Running the Algorithms in a Scaling Study
- in this directory (ygm/examples), salloc the max number of nodes that you need
- run **module load anaconda3**
- create and activate a conda environment
- in strong_/_scaling.py, you can modify the number of compute nodes that you are running on, as well as adding the additional number of buckets/delta arguments if wanting to run on a fixed number of buckets delta step sssp
- run **python strong_\_scaling.py {algorithm executable name: delta_\_sssp or bf_\_sssp}** for a strong scaling study (fixed number of nodes and fixed rmat scale)

## Switching between running on an RMAT generator and graph from CSV
### For the Delta Stepping Algorithm
- to switch from running on an RMAT graph to running on a csv file (that is ordered in acsending order by source node of each edge, comment out line 34 that initializes the rmat scale, uncomment line 33 specifying the path
- then uncomment line 36 which calls the getGraph function, and recomment line 43, which generates the RMAT graph
- close out of the file, **cd ../build** then **make**, salloc the desired number of nodes and then **srun examples/delta_|_sssp {path to graph csv}**

### For the Bellman Ford Algorithm
- comment out the line 26 which initializes the RMAT scale, and uncomment the line 29, which initializes the path to the csv
- comment out line 32 to generate_\_rmat_|_graph, and then uncomment line 31 to getGraph
- close out of the file, **cd ../build** then **make**, salloc the desired number of nodes, and then **srun examples/bf_\_sssp {path to graph csv}**

## Generate RMAT Graphs into a CSV file
- use the rmat_\_gen_\_graphs.csv
- from this directory, **cd ../build** and then **make** and **srun -o {path to desired destination of the csv file}.csv examples/rmat_\_gen_\_graphs n** where n is the rmat scale
