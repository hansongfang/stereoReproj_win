"""F0F1 vs F1F0
"""
import numpy as np
from os.path import join
from utils import readCSV
import matplotlib.pyplot as plt


def getData(renderOptId, rotateId, qualityOffset):
    """
    (renderOptId, rotatePeriodId) -> ([psnr, ssim]
    """
    return data[renderOptId * numRotates + rotateId, 2 + qualityOffset]


def plot_rotatePeriod_quality():
    """line marker graph rotatePeriod -> PSNR"""
    colors = ['C0', 'C1', 'C2', 'C3', 'y', 'C9', 'C7']
    for qualityId in range(2):
        y1 = np.zeros(numRotates, dtype=np.float)
        y2 = np.zeros(numRotates, dtype=np.float)
        for rotateId in range(numRotates):
            y1[rotateId] = getData(0, rotateId, qualityOffset=qualityId)
            y2[rotateId] = getData(1, rotateId, qualityOffset=qualityId)

        plt.plot(rotatePeriod, y1, color='C0', marker='o', linestyle='-', linewidth=2, markersize=6)
        plt.plot(rotatePeriod, y2, color='C1', marker='o', linestyle='-', linewidth=2, markersize=6)
        plt.legend(['F0F1', 'F1F0'])
        plt.xlabel('rotate period')
        plt.ylabel(qualityOpt[qualityId])
        outFile = join(rootDir, modelName[modelId],
                       models[modelName[modelId]][coarseResId] + "_rot{}_".format(renderMode) + qualityOpt[qualityId]+".png")
        plt.savefig(outFile, bbox_inches='tight')
        plt.show()


if __name__=="__main__":
    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Armadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']
    numResolution = 6

    rotatePeriod = np.array([20, 40, 60, 80, 100, 200, 300, 400, 500, 600], dtype=np.float)
    numRotates = rotatePeriod.shape[0]
    qualityOffset = [0, 1]
    qualityOpt = ['PSNR', 'SSIM']

    for modelId in range(4):
        # modelId = 0
        coarseResId = 2
        renderMode = '25'
        rootDir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
        datafile = join(rootDir, modelName[modelId], models[modelName[modelId]][coarseResId] + "_rot1_{}_2.csv".format(renderMode))
        data = readCSV(datafile)
        plot_rotatePeriod_quality()
