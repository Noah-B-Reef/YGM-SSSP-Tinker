import pandas as pd
import csv
import sys

path = sys.argv[1]
 # open file
with open(path, "r") as csvfile:
    data=[tuple(line) for line in csv.reader(csvfile)]
    data.sort()

    df = pd.DataFrame(data, columns=["source", "end", "weight"])
    df.to_csv(path)