#ifndef IMAGEQUALITY_H
#define IMAGEQUALITY_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace cv;

class ImageQuality
{
    Vec3b                    _whitepixel;
    Vec3b                    _greenpixel;
    Vec3b                    _redpixel;

public:
    ImageQuality();
    std::pair<double, double> computeQualityMat(const Mat& mat1, const Mat& mat2, int block_size = 10);

    std::pair<double,double> computeQuality(std::string sfile1,
        std::string sfile2, int block_size=10);

    // This function compute the ratio = green pixels / (green + red pixels)
    // Assumption: image only contrain white/red/green 3 colors
    double greenPixelRatio(std::string file);// reprojection ratio

    std::pair<double,double> errorRatio(std::string labelFile, std::string file);// error
};

#endif // IMAGEQUALITY_H
