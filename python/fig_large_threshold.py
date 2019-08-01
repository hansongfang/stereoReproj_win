import cv2
import numpy as np
from os.path import join

if __name__=="__main__":
    root_dir = "/Users/sfhan/Dropbox/stereoRepoj/largeThreshold"

    img_file= join(root_dir, "right_frame130_16.png")
    img2_file = join(root_dir, "right_frame130_100.png")
    gt_file = join(root_dir, "gt_right_frame130.png")
    out_file = join(root_dir, "largethreshold.png")

    I1 = cv2.imread(img_file)
    I2 = cv2.imread(img2_file)
    gtI = cv2.imread(gt_file)

    x1, y1 = 356, 146
    x2, y2 = 580, 290
    width = x2- x1
    height = y2 - y1

    x3, y3 = 384, 163
    x4, y4 = 519, 246

    mark_I1 = cv2.rectangle(I1, (x3, y3), (x4, y4), color=(0, 255, 00), thickness=3)
    mark_I2 = cv2.rectangle(I2, (x3, y3), (x4, y4), color=(0, 255, 00), thickness=3)

    crop_I1 = mark_I1[y1:y2, x1:x2, :]
    crop_I2 = mark_I2[y1:y2, x1:x2, :]
    crop_gtI = gtI[y1:y2, x1:x2, :]

    outI = np.concatenate((crop_I1, crop_I2, crop_gtI), axis=1)
    cv2.imwrite(out_file, outI.astype(np.uint8))
