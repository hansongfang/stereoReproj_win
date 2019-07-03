import numpy as np
import csv

def readCSV(datafile, startCol=0):
    results = []
    with open(datafile) as csvfile:
        reader = csv.reader(csvfile, quoting=csv.QUOTE_NONNUMERIC)  # change contents to floats
        for row in reader:
            results.append(row)
    nRows = len(results)
    nCols = len(results[0])
    data = np.ones((nRows, nCols))
    for idx in range(nRows):
        for idy in range(startCol, nCols):
            data[idx, idy] = results[idx][idy]
    return data