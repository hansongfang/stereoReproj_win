"""Making directory
Songfang Han
"""

import os
from os.path import join

def sf_mkdir(path):
    if not os.path.isdir(path):
        os.makedirs(path)

def makeDirs(rootDir, threshList):
    for model in modelName:
        modelDir = join(rootDir, model)
        sf_mkdir(modelDir)
        for coarseModel in models[model]:
            resModelDir = join(modelDir, coarseModel)
            sf_mkdir(resModelDir)
            for threshold in threshList:
                threshDir = join(resModelDir, str(threshold))
                sf_mkdir(threshDir)

if __name__== "__main__":
    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Armadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']
    numResolution = 6

    rootDir = "G:/vr/stereoReproj/Results/"
    thresholdList = list(range(2, 24, 2))
    makeDirs(rootDir, thresholdList)