from os.path import join
import cv2
import numpy as np

if __name__=="__main__":
    # o.25, o.5, F0F1, 1.0, gt
    # resize -> crop -> concatenate
    res_dir = "/Users/sfhan/Dropbox/stereoRepoj/boundary/"
    I_file = join(res_dir, "right_frame18_F0_1.0.png")
    I_res_half_file = join(res_dir, "right_frame18_F0_0.5.png")
    I_res_half_F0F1_file = join(res_dir, "right_frame18_F0F1_0.5.png")
    I_res_quater_file = join(res_dir, "right_frame18_F0_0.25.png")
    gt_file = join(res_dir, "gt_right_frame18_1.0.png")
    out_file = join(res_dir, "boundary.png")

    I = cv2.imread(I_file).astype(np.float)
    height, width, _ = I.shape

    I_res_half = cv2.imread(I_res_half_file)
    I_res_half = cv2.resize(I_res_half, (width, height))
    I_res_half_F0F1 = cv2.imread(I_res_half_F0F1_file)
    I_res_half_F0F1 = cv2.resize(I_res_half_F0F1, (width, height))
    I_res_quater = cv2.imread(I_res_quater_file)
    I_res_quater = cv2.resize(I_res_quater, (width, height))
    gtI = cv2.imread(gt_file)
    gtI = cv2.resize(gtI, (width, height))

    x1, y1 = 90, 0
    x2, y2 = 190, 150
    cropI = I[y1: y2, x1:x2, :]
    cropI_half = I_res_half[y1: y2, x1:x2, :]
    cropI_half_F0F1 = I_res_half_F0F1[y1: y2, x1:x2, :]
    cropI_quater = I_res_quater[y1: y2, x1:x2, :]
    crop_gtI = gtI[y1: y2, x1:x2, :]

    outI = np.concatenate((cropI_quater, cropI_half, cropI_half_F0F1, cropI, crop_gtI), axis=1)
    cv2.imwrite(out_file, outI.astype(np.uint8))


