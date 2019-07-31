""" This file is toGraph 3D wireframe, (rot, trans) -> psnr/ssim, legens: leftPrimary/rightPrimary

    Songfang Han
    2018-Dec-09
"""
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

def graph_theta_phi_3d():
    """Graph 3D wireframe, (theta, phi) -> psnr/ssim, legens: leftPrimary/rightPrimary
    """
    ZLeft = np.zeros((len(thetaList), len(phiList)))
    ZRight = np.zeros((len(thetaList), len(phiList)))
    renderOptId = 0
    for qualityId in range(2):
        # set up data
        for thetaId in range(len(thetaList)):
            for phiId in range(len(phiList)):
                dataID = thetaId * len(phiList) + phiId
                colID = 2 + qualityOffset[qualityId]
                ZLeft[thetaId, phiId] = getData(thetaId, phiId, colID)
                colID = 4 + qualityOffset[qualityId]
                ZRight[thetaId, phiId] = getData(thetaId, phiId, colID)

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        # Plot a basic wireframe.
        ax.plot_wireframe(X, Y, ZLeft, rstride=1, cstride=1, color='C1')
        ax.plot_wireframe(X, Y, ZRight, rstride=1, cstride=1)
        # ax.plot_surface(X, Y, ZLeft, rstride=1, cstride=1, cmap='Oranges')
        # ax.plot_surface(X, Y, ZRight, rstride=1, cstride=1, cmap='Blues')

        ax.set_xlabel('phi')
        ax.set_ylabel('theta')
        ax.set_zlabel(qualityOpt[qualityId])
        plt.title(modelName[modelId])
        outFile = models[modelName[modelId]][coarseModelId] + '_theta_phi_' + qualityOpt[qualityId] + '_render2.png'
        outFile = join(rootDir, modelName[modelId], outFile)
        plt.savefig(outFile, bbox_inches='tight')
        plt.show()


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

    thetaList = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180]
    #thetaList = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90]
    phiList = [0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340]
    numPhi = 18
    xVec = np.array(thetaList)
    yVec = np.array(phiList)
    X, Y = np.meshgrid(yVec, xVec)
    qualityOpt = ['  PSNR', '  SSIM']
    qualityOffset = [0, 1]

    modelId = 0
    rootDir = "G:/vr/stereoReproj/Results"
    datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_rotation_renderOrder.csv")
    data = readCSV(datafile)
    graph_theta_phi_3d()