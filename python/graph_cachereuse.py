""" This file is to graph group bar, (resolution, freshcount) -> psnr/ssim

    Songfang Han
    2018-Dec-09
"""
import matplotlib.pyplot as plt
import numpy as np
from os.path import join
from utils import readCSV
import seaborn as sns
from matplotlib.ticker import FormatStrFormatter


def getData(resolutionId, freshOptId, qualityOffset):
    """(resolutionId, freshOptId) -> (psnr, ssim)"""
    return data[resolutionId * freshDim + freshOptId, 2 + qualityOffset]


def plot_freshrate_bar(renderOptData, ylabel, xticks_list):
    """
    :param renderOptData: np.array((numFreshOptions, numResolution))
    :param ylabel: missratio
    :param xticks_list: [1K, 3K, 5K, 10K, 25K]
    :return:
    """
    numFreshOption = len(freshOptIds)
    numCoarseModels = 6
    x = np.arange(numCoarseModels)
    total_width = 0.8
    bar_width = total_width / numFreshOption

    colors = ['C0', 'C1', 'C2', 'C3', 'y', 'C9', 'C7']
    for id in range(numFreshOption):
        plt.bar(x + bar_width * id, renderOptData[id, :], width=bar_width, label=freshLabels[id], color=colors[id])
    low = np.min(renderOptData)
    high = np.max(renderOptData)
    plt.ylim([low - 0.5 * (high - low), high + 0.5 * (high - low)])
    plt.ylabel(ylabel, fontsize=8)
    plt.yticks(fontsize=7)
    plt.xlabel('Resolution', fontsize=8)
    plt.xticks(x + 2.5 * bar_width, xticks_list, fontsize=7)
    plt.legend(fontsize=7, loc='upper left')



def plot_freshrate_groupbar():
    sns.set(style="whitegrid")
    sns.set_context("paper")

    # sns.set(style='darkgrid')
    # sns.set_context("talk")

    """(resolution, freshrate) -> missratio, groupbar"""
    for qualityId in range(2):
        plt.figure(figsize=(4, 3))

        if qualityId == 0:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%d'))
        elif qualityId == 1:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%.3f'))

        qualityLabel = qualityOpt[qualityId]

        renderOptData = np.zeros((len(freshOptIds), numResolution))
        for x in range(len(freshOptIds)):
            freshId = freshOptIds[x]
            for resolutionId in range(numResolution):
                renderOptData[x, resolutionId] = getData(resolutionId, freshId, qualityOffset=qualityId)
        xticks_list = allLegend[modelName[modelId]]
        plot_freshrate_bar(renderOptData, ylabel=qualityLabel, xticks_list=xticks_list)
        # outFile = join(rootDir, modelName[modelId],
        #                modelName[modelId] + "_freshrate_{}.pdf".format(qualityLabel))
        outFile = join(rootDir, modelName[modelId], modelName[modelId] + "_freshrate_refRender_{}.pdf".format(qualityLabel))
        plt.tight_layout()
        plt.savefig(outFile)
        print(outFile)
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

    allLegend = dict()
    allLegend['Lucy'] = ['1K', '3K', '5K', '10K', '25K', '100K']
    allLegend['angel'] = ['1K', '3K', '5K', '10K', '25K', '50K']
    allLegend['Armadillo'] = ['1K', '3K', '5K', '10K', '25K', '35K']
    allLegend['bunny'] = ['1K', '3K', '5K', '10K', '25K', '70K']
    freshOptIds = [0, 1, 2, 3, 4]
    freshDim = 8
    freshLabels = ['Cache1', 'Cache2', 'Cache3', 'Cache4', 'Cache5']
    qualityOpt = ['PSNR', 'SSIM']
    qualityOffset = 2


    for modelId in range(4):
        #modelId = 0
        rootDir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
        # datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_freshcount_F0_quality.csv")
        datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_freshcount_F0_refRender_quality.csv")
        print(datafile)
        data = readCSV(datafile)
        plot_freshrate_groupbar()