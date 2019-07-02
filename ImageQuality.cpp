#include "ImageQuality.h"

#define C1 (float) (0.01 * 255 * 0.01  * 255)
#define C2 (float) (0.03 * 255 * 0.03  * 255)

ImageQuality::ImageQuality()
    :_whitepixel(255, 255, 255)
    , _greenpixel(0, 255, 0)
    ,_redpixel(0, 0, 255)
{
}

// sigma on block_size
double sigma(Mat & m, int i, int j, int block_size)
{
    double sd = 0;

    Mat m_tmp = m(Range(i, i + block_size), Range(j, j + block_size));
    Mat m_squared(block_size, block_size, CV_64F);

    multiply(m_tmp, m_tmp, m_squared);

    // E(x)
    double avg = mean(m_tmp)[0];
    // E(x²)
    double avg_2 = mean(m_squared)[0];


    sd = sqrt(avg_2 - avg * avg);

    return sd;
}

// Covariance
double cov(Mat & m1, Mat & m2, int i, int j, int block_size)
{
    Mat m3 = Mat::zeros(block_size, block_size, m1.depth());
    Mat m1_tmp = m1(Range(i, i + block_size), Range(j, j + block_size));
    Mat m2_tmp = m2(Range(i, i + block_size), Range(j, j + block_size));


    multiply(m1_tmp, m2_tmp, m3);

    double avg_ro = mean(m3)[0]; // E(XY)
    double avg_r = mean(m1_tmp)[0]; // E(X)
    double avg_o = mean(m2_tmp)[0]; // E(Y)


    double sd_ro = avg_ro - avg_o * avg_r; // E(XY) - E(X)E(Y)

    return sd_ro;
}


// Mean squared error
double eqm(Mat & img1, Mat & img2)
{
    int i, j;
    double eqm = 0;
    int height = img1.rows;
    int width = img1.cols;

    int count = 0;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++) {
            eqm += (img1.at<double>(i, j) - img2.at<double>(i, j)) * (img1.at<double>(i, j) - img2.at<double>(i, j));
            count++;
//            if (img1.at<double>(i, j) < 255.0) {
//                eqm += (img1.at<double>(i, j) - img2.at<double>(i, j)) * (img1.at<double>(i, j) - img2.at<double>(i, j));
//                count++;
//            }
        }

    eqm /= count;
    return eqm;
}

double psnr(Mat & img_src, Mat & img_compressed)
{
    int D = 255;
    return (10 * log10((D*D) / eqm(img_src, img_compressed)));
}

Vec3b printIntensity(int i, int j, Mat& img_src)
{
    Vec3b intensity = img_src.at<Vec3b>(i, j);
    uchar blue = intensity.val[0];
    uchar green = intensity.val[1];
    uchar red = intensity.val[2];
    std::cout<<"intensity = "<< intensity<<std::endl;
    std::cout<< (int)blue<<" "<<(int)green<<" "<<(int)red<<std::endl;

    return intensity;
}

// This function compute the ratio = green pixels / (green + red pixels)
// Assumption: image only contrain white/red/green 3 colors
double ImageQuality::greenPixelRatio(std::string file)
{
    Mat img_src;
    img_src = imread(file, CV_LOAD_IMAGE_COLOR);// load bgr
    if (!img_src.data) {
        std::cout << "could not load image file " << file << std::endl;
    }

    img_src.convertTo(img_src,CV_8UC3);
    Vec3b temp;

    int modelpixels = 0;int redPixels = 0;
    int height = img_src.rows;
    int width = img_src.cols;
    for(int i=0; i < height; i++){
        for(int j=0;j<width;j++){
            temp = img_src.at<Vec3b>(i, j);
            if (temp != _whitepixel){
                modelpixels ++;
                //test if it is red or not
                if((int)temp.val[2] != 0 && (int)temp.val[0] == 0 && (int)temp.val[1] == 0){
                    redPixels ++;
                }
            }
        }
    }
    return (double)(modelpixels-redPixels)/(double)modelpixels;
}


double ssim(Mat & img_src, Mat & img_compressed, int block_size, bool show_progress = false)
{
    double ssim = 0;

    int nbBlockPerHeight = img_src.rows / block_size;
    int nbBlockPerWidth = img_src.cols / block_size;

    for (int k = 0; k < nbBlockPerHeight; k++)
    {
        for (int l = 0; l < nbBlockPerWidth; l++)
        {
            int m = k * block_size;
            int n = l * block_size;

            double avg_o = mean(img_src(Range(k, k + block_size), Range(l, l + block_size)))[0];
            double avg_r = mean(img_compressed(Range(k, k + block_size), Range(l, l + block_size)))[0];
            double sigma_o = sigma(img_src, m, n, block_size);
            double sigma_r = sigma(img_compressed, m, n, block_size);
            double sigma_ro = cov(img_src, img_compressed, m, n, block_size);

            ssim += ((2 * avg_o * avg_r + C1) * (2 * sigma_ro + C2)) / ((avg_o * avg_o + avg_r * avg_r + C1) * (sigma_o * sigma_o + sigma_r * sigma_r + C2));

        }
        // Progress
        if (show_progress)
            std::cout << "\r>>SSIM [" << (int)((((double)k) / nbBlockPerHeight) * 100) << "%]";
    }
    ssim /= nbBlockPerHeight * nbBlockPerWidth;

    if (show_progress)
    {
        std::cout << "\r>>SSIM [100%]" << std::endl;
        std::cout << "SSIM : " << ssim << std::endl;
    }

    return ssim;
}

std::string type2str(int type) {
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

std::pair<double,double> ImageQuality::computeQuality(std::string sfile1,
    std::string sfile2, int block_size)
{
    Mat img_src;
    Mat img_compressed;

    img_src = imread(sfile1, CV_LOAD_IMAGE_GRAYSCALE);
    img_compressed = imread(sfile2, CV_LOAD_IMAGE_GRAYSCALE);

    img_src.convertTo(img_src, CV_64F);
    img_compressed.convertTo(img_compressed, CV_64F);
    int height_o = img_src.rows;
    int height_r = img_compressed.rows;
    int width_o = img_src.cols;
    int width_r = img_compressed.cols;

    // Check pictures size
    if (height_o != height_r || width_o != width_r)
    {
        std::cout << "Images must have the same dimensions" <<std::endl;
        return std::make_pair(0.0,0.0);
    }

    // Check if the block size is a multiple of height / width
    if (height_o % block_size != 0 || width_o % block_size != 0)
    {
        /*std::cout << "WARNING : Image WIDTH and HEIGHT should be divisible by BLOCK_SIZE for the maximum accuracy" << std::endl
            << "HEIGHT : " << height_o << std::endl
            << "WIDTH : " << width_o << std::endl
            << "BLOCK_SIZE : " << block_size << std::endl
            << std::endl;*/
    }

    double ssim_val = ssim(img_src, img_compressed, block_size);
    double psnr_val = psnr(img_src, img_compressed);

    return std::make_pair(ssim_val,psnr_val);
}

std::pair<double, double> ImageQuality::computeQualityMat(const Mat& mat1, const Mat& mat2, int block_size)
{
    Mat repMat = mat1.clone();
    Mat gdMat = mat2.clone();

    cv::Mat img_src, img_compressed;
    cv::cvtColor(repMat, img_src, CV_BGR2GRAY);
    cv::cvtColor(gdMat, img_compressed, CV_BGR2GRAY);
    img_src.convertTo(img_src, CV_64F);
    img_compressed.convertTo(img_compressed, CV_64F);

    int height_o = img_src.rows;
    int height_r = img_compressed.rows;
    int width_o = img_src.cols;
    int width_r = img_compressed.cols;

    // Check pictures size
    if (height_o != height_r || width_o != width_r)
    {
        std::cout << "Images must have the same dimensions" << std::endl;
        return std::make_pair(0.0, 0.0);
    }

    // Check if the block size is a multiple of height / width
    if (height_o % block_size != 0 || width_o % block_size != 0)
    {
        /*std::cout << "WARNING : Image WIDTH and HEIGHT should be divisible by BLOCK_SIZE for the maximum accuracy" << std::endl
            << "HEIGHT : " << height_o << std::endl
            << "WIDTH : " << width_o << std::endl
            << "BLOCK_SIZE : " << block_size << std::endl
            << std::endl;*/
    }

    double ssim_val = ssim(img_src, img_compressed, block_size);
    double psnr_val = psnr(img_src, img_compressed);

    return std::make_pair(ssim_val, psnr_val);

}
/* This function is to measure two errorRatio
Args:
    labelFile: green pixel for model pixel, white pixel for background
    file: green pixel for reprojection, red pixel for failed reprojection, white for background

Output:
    redpixel(file)&modelpixel/modelpixel
    greenpixel(file)&no_modelpixel/modelpixel

Assumption:
   greenColor // reprojeciton or model pixel
   redColor // failed reprojection
   whiteColor // background color
*/
std::pair<double, double> ImageQuality::errorRatio(std::string labelFile, std::string file)
{
    Mat labelI, I;

    labelI = imread(labelFile, CV_LOAD_IMAGE_COLOR);
    labelI.convertTo(labelI, CV_8UC3);
    I = imread(file, CV_LOAD_IMAGE_COLOR);
    I.convertTo(I, CV_8UC3);


    int height, width;
    height = labelI.rows;
    width = labelI.cols;

    int modelPixelCount=0;
    int errorCount1=0; // modelpixel & red
    int errorCount2=0; // no model pixel & green
    Vec3b temp, temp2;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp = labelI.at<Vec3b>(i, j);
            temp2 = I.at<Vec3b>(i, j);
            if (temp == _greenpixel) {
                modelPixelCount++;
                if (temp2 == _redpixel) {
                    errorCount1++;
                }
            }
            else {
                if (temp2 == _greenpixel) {
                    errorCount2++;
                }
            }
        }
    }
    double errorRatio1 = (double)(errorCount1) / (double)modelPixelCount;
    double errorRatio2 = (double)(errorCount2) / (double)modelPixelCount;
    return std::make_pair(errorRatio1, errorRatio2);
}
