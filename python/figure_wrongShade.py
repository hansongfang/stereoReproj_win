from os.path import join
import cv2
import numpy as np

if __name__=="__main__":
    res_dir = "/Users/sfhan/Dropbox/stereoRepoj/wrongShade/"
    reprojI_F0_file = join(res_dir, "right_frame24_F0.png")
    reprojI_F0_green_file = join(res_dir, "right_frame24_F0_green.png")
    reprojI_F0F1_file = join(res_dir, "right_frame24_F0F1.png")
    gt_file = join(res_dir, "gt_right_frame24.png")
    out_file = join(res_dir, "wrongShade.png")

    reprojI_F0 = cv2.imread(reprojI_F0_file).astype(np.float)
    reprojI_F0_green = cv2.imread(reprojI_F0_green_file).astype(np.float)
    reprojI_F0F1 = cv2.imread(reprojI_F0F1_file).astype(np.float)
    gtI = cv2.imread(gt_file).astype(np.float)

    x1, y1 = 345, 45
    x2, y2 = 600, 300

    crop_reprojI_F0 = reprojI_F0[y1: y2, x1:x2, :]
    crop_reprojI_F0_green = reprojI_F0_green[y1: y2, x1:x2, :]
    crop_reprojI_F0F1 = reprojI_F0F1[y1: y2, x1:x2, :]
    crop_gtI = gtI[y1: y2, x1:x2, :]

    # 2 * 2 format
    row1 = np.concatenate((crop_reprojI_F0, crop_reprojI_F0_green), axis=1)
    row2 = np.concatenate((crop_reprojI_F0F1, crop_gtI), axis=1)
    out = np.concatenate((row1, row2), axis=0)

    cv2.imwrite(out_file, out.astype(np.uint8))