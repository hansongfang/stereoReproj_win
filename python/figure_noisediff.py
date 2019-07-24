import os
from utils import colorDiff
from os.path import join
import cv2
import numpy as np

if __name__=="__main__":
    res_dir = "/Users/sfhan/Dropbox/stereoRepoj/noiseShadingQuality/"
    gtI_file = join(res_dir, "gt_right_frame7.png")
    renderI_file = join(res_dir, "right_frame7.png")
    diff_file = join(res_dir, "diff.png")
    out_file = join(res_dir, "noise.png")

    gtI = cv2.imread(gtI_file).astype(np.float)
    renderI = cv2.imread(renderI_file).astype(np.float)

    diffI = colorDiff(gtI, renderI, diff_max=255.0, diff_min=0.0) * 255.0
    diffI = diffI[:, :, ::-1]
    cv2.imwrite(diff_file, diffI.astype(np.uint8))

    x1, y1 = 215, 105
    x2, y2 = 590, 845
    width = x2 - x1
    height = y2 - y1
    crop_gtI = gtI[y1: y2, x1:x2, :]
    crop_renderI = renderI[y1: y2, x1:x2, :]
    crop_diffI = diffI[y1: y2, x1:x2, :]
    outI = np.concatenate((crop_renderI, crop_gtI, crop_diffI), axis=1)

    cv2.imwrite(out_file, outI.astype(np.uint8))

