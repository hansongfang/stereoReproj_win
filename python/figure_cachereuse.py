from os.path import join
import cv2
import numpy as np
from utils import colorDiff


if __name__=="__main__":
    res_dir = "/Users/sfhan/Dropbox/stereoRepoj/cacheReuse_cache12cache2/"
    coarseDI_file = join(res_dir, "Lucy5k_left_frame2_coarseDepth_F0.png")
    fineDI_file = join(res_dir, "Lucy5k_left_frame2_fineDepth_F0.png")
    gt_file = join(res_dir, "gt_left_frame2.png")
    out_file1 = join(res_dir, "eq6.png")
    out_file2 = join(res_dir, "eq6_diff.png")
    diff_file = join(res_dir, "cachereuseDiff.png")

    coarseDI = cv2.imread(coarseDI_file).astype(np.float)
    fineDI = cv2.imread(fineDI_file).astype(np.float)
    gtI = cv2.imread(gt_file).astype(np.float)

    # version1 coarseDI, fineDI, gtI
    x1, y1 = 200, 0
    x2, y2 = 740, 1080
    crop_coarseDI = coarseDI[y1:y2, x1:x2, :]
    crop_fineDI = fineDI[y1:y2, x1:x2, :]
    crop_gtI = gtI[y1:y2, x1:x2, :]
    out_I_v1 = np.concatenate((crop_coarseDI, crop_fineDI, crop_gtI), axis=1)
    cv2.imwrite(out_file1, out_I_v1.astype(np.uint8))


    crop_diff_coarseDI = colorDiff(crop_gtI, crop_coarseDI, diff_max=255.0, diff_min=0.0) * 255.0
    crop_diff_coarseDI = crop_diff_coarseDI[:, :, ::-1]
    crop_diff_fineDI = colorDiff(crop_gtI, crop_fineDI, diff_max=255.0, diff_min=0.0) * 255.0
    crop_diff_fineDI = crop_diff_fineDI[:, :, ::-1]
    crop_diff_fineD_coarseDI = colorDiff(crop_coarseDI, crop_fineDI, diff_max=255.0, diff_min=0.0) * 255.0
    crop_diff_fineD_coarseDI = crop_diff_fineD_coarseDI[:, :, ::-1]
    crop_diffIs = np.concatenate((crop_diff_coarseDI, crop_diff_fineDI, crop_diff_fineD_coarseDI), axis=1)
    cv2.imwrite(diff_file, crop_diffIs)

    out_I_v2 = np.concatenate((out_I_v1, crop_diffIs), axis=0)
    cv2.imwrite(out_file2, out_I_v2.astype(np.uint8))


