import numpy as np
import csv
import matplotlib
import matplotlib.pyplot as plt

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

def norm_2d(x, max, min):
    y = np.clip(x, min, max)
    y = (y-min)/(max - min)
    return y


def colorDiff(gtI, I, diff_max=255.0, diff_min=0.0):
    cmap = plt.get_cmap(name="jet")
    diff_img = np.abs(gtI - I)
    diff_img = np.mean(diff_img, axis=2)
    diff_img = norm_2d(diff_img, diff_max, diff_min)
    diff_img = cmap(diff_img)[:, :, :3]
    return diff_img
