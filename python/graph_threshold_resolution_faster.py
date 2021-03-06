""" This file is toGraph 3D wireframe, (rot, trans) -> psnr/ssim, legens: leftPrimary/rightPrimary

    Songfang Han
    2018-Dec-09
"""
import matplotlib.pyplot as plt
import numpy as np
from os.path import join
from utils import readCSV
import seaborn as sns
from matplotlib.ticker import FormatStrFormatter

def getData(renderOptId, thresholdId, resolutionId, qualityOffset):
    """
    (renderOpt, threshold) -> ([psnr, ssim]_1k, [psnr ,ssim]_3k)
    """
    numThreshold = len(thresholds)
    return data[renderOptId * numThreshold + thresholdId, 2 + resolutionId*2 + qualityOffset]


def plot_threshold_resolution(renderOptId):
    sns.set(style="whitegrid")
    sns.set_context("paper")

    colors = ['C0', 'C1', 'C2', 'C3', 'y', 'C9', 'C7']
    thresholdX = np.array(thresholds)
    numThreshold = len(thresholdX)
    for qualityId in range(2):
        fig = plt.figure(figsize=(4, 3))

        if qualityId == 0:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%d'))
        elif qualityId == 1:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%.3f'))


        qualityOffset = qualityId
        for resolutionId in range(numResolution):
            colId = 2 + resolutionId * 2 + qualityOffset
            tempData = data[renderOptId * numThreshold: (renderOptId+1)*numThreshold, colId]
            plt.plot(thresholdX, tempData, color=colors[resolutionId], linestyle='-', linewidth=1)
        plt.legend(allLegend[modelName[modelId]], fontsize=8)
        plt.xticks(fontsize=7)
        # plt.title(modelName[modelId])
        plt.xlabel("Threshold", fontsize=8)
        plt.yticks(fontsize=7)
        plt.ylabel(qualityOpt[qualityId], fontsize=8)
        outFile = join(rootDir, modelName[modelId],modelName[modelId] + "_" + qualityOpt[qualityId] +
                       "_renderOpt" + str(renderOptId) + "_faster.pdf")
        # outFile = join(rootDir, modelName[modelId],
        #                modelName[modelId] + "_" + qualityOpt[qualityId] + "_renderOpt" + str(renderOptId) + ".pdf")

        plt.tight_layout()
        print(outFile)
        plt.savefig(outFile)
        plt.show()


def plot_renderOption_bar(renderOptData, ylabel, xticks_list):

    numRenderOption = len(renderOpts)
    numCoarseModels = 6
    x = np.arange(numCoarseModels)
    total_width = 0.8
    bar_width = total_width / numRenderOption

    colors = ['C0', 'C1', 'C2', 'C3', 'y', 'C9', 'C7']
    for id in range(numRenderOption):
        plt.bar(x + bar_width * id, renderOptData[id, :], width=bar_width, label=renderLabels[id], color=colors[id])
    low = np.min(renderOptData)
    high = np.max(renderOptData)
    plt.ylim([low - 0.5 * (high - low), high + 0.5 * (high - low)])
    plt.ylabel(ylabel, fontsize=8)
    plt.yticks(fontsize=7)
    plt.xlabel('Resolution', fontsize=8)
    plt.xticks(x + 2.5 * bar_width, xticks_list, fontsize=8)
    plt.legend(fontsize=7)


def plot_renderOption(thresholdId):
    sns.set(style="whitegrid")
    sns.set_context("paper")

    numRenderOption = len(renderOpts)
    for qualityId in range(2):
        plt.figure(figsize=(4, 3))

        if qualityId == 0:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%d'))
        elif qualityId == 1:
            ax = plt.gca()
            ax.yaxis.set_major_formatter(FormatStrFormatter('%.3f'))

        renderOptData = np.zeros((numRenderOption, numResolution))
        for renderOptId in range(numRenderOption):
            for resolutionId in range(numResolution):
                renderOptData[renderOptId, resolutionId] = \
                    getData(renderOptId, thresholdId, resolutionId, qualityId)
        xticks_list = allLegend[modelName[modelId]]
        plot_renderOption_bar(renderOptData, ylabel=qualityOpt[qualityId], xticks_list=xticks_list)
        outFile = join(rootDir, modelName[modelId],
                       modelName[modelId] + "_renderOption_" + qualityOpt[qualityId] + "_faster.pdf")
        # outFile = join(rootDir, modelName[modelId],
        #                modelName[modelId] + "_renderOption_" + qualityOpt[qualityId] + ".pdf")
        plt.tight_layout()
        print(outFile)
        plt.savefig(outFile)
        plt.show()


if __name__== "__main__":
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
    thresholds = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]) * 0.0002
    renderOpts = [0, 1, 2, 3, 4]
    renderLabels = ['F1', 'F0', 'F0F1', 'F0F1F2', 'F0F1F2F3']
    qualityOpt = ['PSNR', 'SSIM']
    qualityOffset = [0, 1]

    for modelId in range(4):
    # modelId = 3
        resolutionId = 2
        rootDir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
        # datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_threshold_renderOption2.csv")
        datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_threshold_renderOption_faster2.csv")
        data = readCSV(datafile)
        plot_threshold_resolution(1)

        modelThreshold = dict()
        modelThreshold['Lucy'] = 12 // 2
        modelThreshold['angel'] = 16 // 2
        modelThreshold['Armadillo'] = 18 // 2
        modelThreshold['bunny'] = 12 // 2
        plot_renderOption(modelThreshold[modelName[modelId]])