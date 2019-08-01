""" This file is toGraph 3D wireframe, (rot, trans) -> psnr/ssim, legens: leftPrimary/rightPrimary

    Songfang Han
    2018-Dec-09
"""
import matplotlib.pyplot as plt
import numpy as np
from os.path import join
from utils import readCSV

# TODO: F0, F1 renderOpt=0 -> F1 renderOpt=1->F0


def getData(renderOptId, resolutionId, qualityOffset):
    """(resolutionId, renderOpt) -> (psnr, ssim)"""
    numRenderOption = len(renderOpts)
    return data[resolutionId * numRenderOption + renderOptId, 2 + qualityOffset]


def plot_renderOption_bar(renderOptData, ylabel, xticks_list):
    """
    :param renderOptData: np.array((numRenderOption, numResolution))
    :param ylabel: missratio
    :param xticks_list: [1K, 3K, 5K, 10K, 25K]
    :return:
    """
    numRenderOption = len(renderOpts)
    numCoarseModels = 6
    x = np.arange(numCoarseModels)
    total_width = 0.8
    bar_width = total_width / numRenderOption

    colors = ['C0', 'C1', 'C2', 'C3', 'y', 'C9', 'C7']
    for id in range(numRenderOption):
        plt.bar(x + bar_width * id, renderOptData[id, :], width=bar_width, label=renderLabels[id], color=colors[id])
    plt.xlabel('resolution')
    low = np.min(renderOptData)
    high = np.max(renderOptData)
    plt.ylim([0.0, high + 0.5 * (high - low)])
    plt.ylabel(ylabel)
    # plt.title(title)
    plt.xticks(x + 2.5 * bar_width, xticks_list)
    plt.legend()

def plot_missratio_groupbar():
    """(resolution, renderOption) -> missratio, groupbar"""
    numRenderOption = len(renderOpts)
    renderOptData = np.zeros((numRenderOption, numResolution))
    for renderOptId in range(numRenderOption):
        for resolutionId in range(numResolution):
            renderOptData[renderOptId, resolutionId] = \
                getData(renderOptId, resolutionId, qualityOffset=qualityOffset)
    xticks_list = allLegend[modelName[modelId]]
    plot_renderOption_bar(renderOptData, ylabel='missratio', xticks_list=xticks_list)
    outFile = join(rootDir, modelName[modelId],
                   modelName[modelId] + "_missratio_faster_earlyZ.png")
    plt.savefig(outFile, bbox_inches='tight')
    plt.show()

    pass

if __name__=="__main__":
    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Armadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']
    numResolution = 6

    allLegend = dict()
    allLegend['Lucy'] = ['1k', '3k', '5k', '10k', '25k', '100k']
    allLegend['angel'] = ['1k', '3k', '5k', '10k', '25k', '50k']
    allLegend['Armadillo'] = ['1k', '3k', '5k', '10k', '25k', '35k']
    allLegend['bunny'] = ['1k', '3k', '5k', '10k', '25k', '70k']
    thresholds = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]) * 0.0002
    renderOpts = [0, 1, 2, 3, 4]
    renderLabels = ['F1', 'F0', 'F0F1', 'F0F1F2', 'F0F1F2F3']
    qualityOpt = ['missratio']
    qualityOffset = 2 # missratiog

    for modelId in range(4):
        #modelId = 0
        resolutionId = 2
        rootDir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
        datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_renderOption_missratio_faster_earlyZ.csv")
        data = readCSV(datafile)

        plot_missratio_groupbar()