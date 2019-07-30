import plotly as py
import plotly.graph_objects as go
import numpy as np
from os.path import join
from utils import readCSV

def getData(thetaId, phiId, colID):
    """
        (theta, phi)
        """
    print(thetaId*numPhi + phiId)
    return data[thetaId*numPhi +phiId, colID]

if __name__=="__main__":
    modelId = 0
    coarseModelId = 2

    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Ardmadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']

    thetaList = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120,  130, 140, 150, 160, 170, 180]
    phiList = [0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340]
    numPhi = len(phiList)
    numTheta = len(thetaList)
    qualityOpt = ['  PSNR', '  SSIM']
    qualityOffset = [0, 1]

    modelId = 0
    coarseModelId = 2
    rootDir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
    datafile = join(rootDir, modelName[modelId], modelName[modelId] + "_model_rotation_renderOrder.csv")
    data = readCSV(datafile)

    offset = 2
    zdata_renderLeft = data[:, offset].reshape((numTheta, numPhi))
    zdata_renderRight = data[:, offset+2].reshape(numTheta, numPhi)
    xdata = np.array(phiList).astype(np.float) #/ 180.0 * np.pi
    ydata = np.array(thetaList).astype(np.float) #/ 180.0 * np.pi

    out_renderLeft = np.ones((numTheta+1, numPhi+1))
    out_renderRight = np.ones((numTheta+1, numPhi+1))
    out_renderLeft[0, 1:] = xdata
    out_renderLeft[1:, 0] = ydata
    out_renderLeft[1:, 1:] = zdata_renderLeft

    np.savetxt("left.csv", out_renderLeft, delimiter=",", fmt="%.4f")
    np.savetxt("right.csv", zdata_renderLeft, delimiter=",", fmt="%.4f")

    layout = go.Layout(
        title="{}".format(models[modelName[modelId]][coarseModelId]),
        autosize=False,
        showlegend=True,
        width=500,
        height=500,
        margin=dict(l=0, r=0, b=0, t=20),
        scene=dict(
            xaxis_title='phi',
            yaxis_title='theta',
            zaxis_title='PSNR',
            # xaxis = dict(
            #     ticktext=['0', 'PI/2', 'PI'],
            #     tickvals=[0, 90, 180]),
            )
    )

    trace1 = go.Surface(x=xdata,
                        y=ydata,
                        z=zdata_renderLeft,
                        showscale=False,
                        opacity=1.0,
                        name="data1",
                        colorscale=[[0, 'rgb(255,0,255)'],
                                    [1, 'rgb(255,0,255)']],
                        )

    trace2 = go.Surface(x=xdata,
                        y=ydata,
                        z=zdata_renderRight,
                        showscale=False,
                        opacity=1.0,
                        name="data2",
                        colorscale=[[0, 'rgb(0,0,255)'],
                                    [1, 'rgb(0,0,255)']],
                        )

    fig = go.Figure(data=[trace1, trace2], layout=layout)
    fig.show()


