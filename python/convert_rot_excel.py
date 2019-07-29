from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
import numpy as np
from os.path import join
from utils import readCSV

def getData(thetaId, phiId, colID):
    """
        (theta, phi)
        """
    print(thetaId*numPhi + phiId)
    return data[thetaId*numPhi +phiId, colID]

if __name__=='__main__':
    modelId = 0
    coarseModelId = 2

    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Armadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']

    # read Data
    # rotatePeriods = [50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300]
    # transPeriods = [50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300]
    # xVec = np.array(rotatePeriods)
    # yVec = np.array(transPeriods)
    # X, Y = np.meshgrid(xVec, yVec)
    # qualityOpt = ['  PSNR', '  SSIM']
    # qualityOffset = [0, 1]

    # thetaList = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180]
    thetaList = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90]
    phiList = [0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340]
    numPhi = 18
    numTheta = 10
    xVec = np.array(thetaList)
    yVec = np.array(phiList)

    out_array = np.array((numTheta+1, numPhi+1))
    out_file = join()
    for thetaId in range(numTheta):
        for phiId in range(phiId):
            out_array[thetaId][phiId] = getData()

    # write to csv
    # send csv to github -> codepen

    # How to write it???
