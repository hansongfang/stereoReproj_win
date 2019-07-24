import os
from os.path import join

if __name__=="__main__":
    modelName = ['Lucy', 'angel', 'Armadillo', 'bunny']
    models = dict()
    models['Lucy'] = ['Lucy1k_o', 'Lucy3k_o', 'Lucy5k_o', 'Lucy10k_o', 'Lucy25k_o', 'Lucy100k']
    models['angel'] = ['angel1k_o', 'angel3k_o', 'angel5k_o', 'angel10k_o', 'angel25k_o', 'angel50k']
    models['Armadillo'] = ['Armadillo1k_o', 'Armadillo3k_o', 'Armadillo5k_o', 'Armadillo10k_o',
                           'Armadillo25k_o', 'Armadillo35k']
    models['bunny'] = ['bunny1k_o', 'bunny3k_o', 'bunny5k_o', 'bunny10k_o', 'bunny25k_o', 'bunny70k']
    numResolution = 6

    result_dir = "/Users/sfhan/Dropbox/stereoRepoj/Results"
    out_dir = "/Users/sfhan/Dropbox/stereoRepoj/Results/oneEyeOneRenderCacheReuseSlow"
    out_file = "mkvideo.sh"
    f = open(out_file, "w")


    for modelId in range(4):
        for resId in range(6):
            video_name = "{}/{}_{}.mp4".format(out_dir, modelName[modelId], models[modelName[modelId]][resId])
            input_folder = "{}/{}/{}/16/left_frame%d.png".format(result_dir, modelName[modelId],
                                                                 models[modelName[modelId]][resId])
            cmd = "ffmpeg -r 30 -i {} -vcodec libx264 -crf 25 -pix_fmt yuv420p {}".format(input_folder, video_name)
            print(cmd)
            f.write("{}\n".format(cmd))
    f.close()
