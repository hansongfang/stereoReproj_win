#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "ImageQuality.h"
#include "Mesh.h"
#include "GLWindow.h"
#include "Shader.h"
#include "ReprojMT.h"
#include "util.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cxxopts.hpp"

using namespace std;

string SHADERPATH = "G:/vr/stereoReproj/shaders/";
//const int WINDOWWIDTH = 840;
//const int WINDOWHEIGHT = 939;
const int WINDOWWIDTH = 960;
const int WINDOWHEIGHT = 1080;

int main(int argc, char* argv[])
{
	cxxopts::Options options("MyProgram", "This program implement coarse reprojection");
	options.add_options()
		("d,debug", "Enable debugging")
		("m,model", "model id", cxxopts::value<int>())
		("c,coarsemodel", "coarse model id", cxxopts::value<int>())
		("r,root", "directory to save results", cxxopts::value<std::string>())
		("p,loopx", "loopx", cxxopts::value<int>())
		("q,loopy", "loopy", cxxopts::value<int>())
		;
	auto result = options.parse(argc, argv);
	int modelId = result["model"].as<int>();
	int coarseResId = result["coarsemodel"].as<int>();
	cout << "modelId " << modelId << endl;
	/*
	auto dir = result["root"].as<string>();
	int loopx = result["loopx"].as<int>();
	int loopy = result["loopy"].as<int>();*/

    //CONSTANTS
    string MODELDIR = "G:/PLY/";
    string RESULTDIR = "G:/vr/stereoReproj/Results/";
    vector<string> MODELNAMES = {"Lucy", "angel", "Armadillo", "bunny"};
    map<int, vector<string>> MODELS;
    MODELS[0] = vector<string>{ "Lucy1k_o", "Lucy3k_o", "Lucy5k_o", "Lucy10k_o", "Lucy25k_o", "Lucy100k" };
    MODELS[1] = vector<string>{ "angel1k_o", "angel3k_o", "angel5k_o", "angel10k_o", "angel25k_o", "angel50k" };
    MODELS[2] = vector<string>{ "Armadillo1k_o", "Armadillo3k_o", "Armadillo5k_o", "Armadillo10k_o",
                                "Armadillo25k_o", "Armadillo35k" };
    MODELS[3] = vector<string>{ "bunny1k_o", "bunny3k_o", "bunny5k_o", "bunny10k_o", "bunny25k_o", "bunny70k" };

    if(0){
        //-----------------------------input---------------------------------------------------------//
        //int modelId = 0;
		int oriResId = 5;
        string fmodelPath = MODELDIR + MODELNAMES[modelId] +"/"+ MODELS[modelId][oriResId]+".ply";
        string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
        string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
        int numFrames = 30;

        int numTargets = 4;
        ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
        reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
        reprojMT.setPath(1, 1, 2);
        //reprojMT.setPath3(1, 1, 2, 200, 30, 10);

        //-------------------render option----------------------------------//
        int renderOptId =2;
        float thresholdVal = 0.0016;
        bool leftPrimary = true;
        bool enableFlip = false;
        bool debug = true;
        bool measureQuality = true;
        reprojMT.updateQuality(measureQuality);
        reprojMT.updateRenderOption(renderOptId);
        reprojMT.renderReprojMT(thresholdVal, leftPrimary, enableFlip, debug);
		//reprojMT.renderReprojMT(thresholdVal, !leftPrimary, enableFlip, debug);
    }
	if (0) {
		//-----------------------------input---------------------------------------------------------//
		//int modelId = 0;
		int oriResId = 5;
		string fmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][oriResId] + ".ply";
		string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
		string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
		int numFrames = 10;

		int numTargets = 4;
		ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
		reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
		reprojMT.setPath(1, 1, 2);
		//        reprojMT.setPath3(1, 1, 2, 200, 30, 10);

		//-------------------render option----------------------------------//
		int renderOptId = 2;
		float thresholdVal = 0.0016;
		bool leftPrimary = true;
		bool enableFlip = false;
		bool debug = false;
		bool measureQuality = true;
		reprojMT.updateQuality(measureQuality);
		reprojMT.updateRenderOption(renderOptId);

		vector<float> thetaList = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120,  130, 140, 150, 160, 170, 180 };
		vector<float> phiList = { 0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340 };
		int numTheta = 19;
		int numPhi = 18;
		int numRows = numTheta * numPhi;
		int numCols = 2 + 4;
		vector<vector<double>> qualityTable(numRows, vector<double>(numCols, 0.0));
		for (int i = 0; i < numTheta; i++) {
			float theta = thetaList[i];
			for (int j = 0; j < numPhi; j++) {
				float phi = phiList[j];
				reprojMT.updateThetaPhi(theta, phi);
				auto res = reprojMT.renderReprojMT(thresholdVal, leftPrimary, enableFlip, debug);
				qualityTable[i * numPhi + j][0] = theta;
				qualityTable[i * numPhi + j][1] = phi;
				qualityTable[i * numPhi + j][2] = res[0];
				qualityTable[i * numPhi + j][3] = res[1];
				res = reprojMT.renderReprojMT(thresholdVal, !leftPrimary, enableFlip, debug);
				qualityTable[i * numPhi + j][4] = res[0];
				qualityTable[i * numPhi + j][5] = res[1];
			}
		}
		string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_rotation_renderOrder.csv";
		cout << "save excel " << ofileName << endl;
		ofstream ofile(ofileName);
		write_csv(qualityTable, numRows, numCols, ofile);
		ofile.close();
	}
    if(1)
    {
        //(renderOpt, threshold) -> [(PSNR, SSIM)_1k, (PSNR, SSIM)_3k, (PSNR, SSIM)_5k, (PSNR, SSIM)_10k]
        // Linear, Nearest Sampling have an effect on the result
        // # of frames also have an effect
        //-----------------------------input---------------------------------------------------------//
        //int modelId = 0;
        int coarseResId = 3, oriResId = 5;
        string fmodelPath = MODELDIR + MODELNAMES[modelId] +"/"+ MODELS[modelId][oriResId]+".ply";
        string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
        string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
        int numFrames = 300;
        int numTargets = 4;

        // resolution_threshold_renderMode_psnr_ssim_table
        // resolution_threshold_renderMode_time_missratio_table
        ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
        reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
        reprojMT.setPath(1, 1, 2);

        //-------------------render option----------------------------------//
        bool leftPrimary = true;
        bool enableFlip = false;
        bool debug = false;
        bool measureQuality = true;
        reprojMT.updateQuality(measureQuality);

        //parameters
        int numCoarseModels = 6;
        float initThreshold = 0.0002;
        float thresholdStep = 0.0002;
        int numThresholds = 12;
        vector<int> renderOptions = {0, 1, 2, 3, 4 };
        int numRenderOption = renderOptions.size();
        int numRows = numRenderOption * numThresholds;
        int numCols = 2 + numCoarseModels * 2;
        vector<vector<double>> qualityTable(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 2, 0.0));
//        vector<vector<double>> qualityTable2(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 3, 0.0));
        for (int renderOptId = 0; renderOptId < numRenderOption; renderOptId++) {
            int renderOption = renderOptions[renderOptId];
            reprojMT.updateRenderOption(renderOption);
            for (int coarseModelId = 0; coarseModelId < numCoarseModels; coarseModelId++) {
                outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + "/";
                cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + ".ply";
                cout << "coarse model " << cmodelPath << endl;
                reprojMT.updateCoarseModel(cmodelPath);
                reprojMT.updateDirectory(outDir);
                for (int i = 0; i < numThresholds; i++) {
                    float thresholdVal = initThreshold + i * thresholdStep;
                    auto res = reprojMT.renderReprojMT(thresholdVal, leftPrimary, enableFlip, debug);
                    qualityTable[renderOptId * numThresholds + i][0] = renderOption;
                    qualityTable[renderOptId * numThresholds + i][1] = thresholdVal;
                    qualityTable[renderOptId * numThresholds + i][2 + coarseModelId * 2] = res[0];
                    qualityTable[renderOptId * numThresholds + i][3 + coarseModelId * 2] = res[1];

//                    qualityTable2[renderOptId * numThresholds + i][0] = renderOption;
//                    qualityTable2[renderOptId * numThresholds + i][1] = thresholdVal;
//                    qualityTable2[renderOptId * numThresholds + i][2 + coarseModelId * 3] = res[6];
//                    qualityTable2[renderOptId * numThresholds + i][3 + coarseModelId * 3] = res[7];
//                    qualityTable2[renderOptId * numThresholds + i][4 + coarseModelId * 3] = res[8];

                    if (1) {
                        cout << "result " << endl;
                        for (auto v : res) {
                            cout << v << " ";
                        }
                        cout << endl;
                    }
                }
            }
        }

        string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption2.csv";
        cout <<"save excel " <<ofileName << endl;
        ofstream ofile(ofileName);
        write_csv(qualityTable, numRows, numCols, ofile);
        ofile.close();

//        ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption_time.csv";
//        cout << "saving excel "<<ofileName << endl;
//        ofile.open(ofileName);
//        write_csv(qualityTable2, numRows, numCols_time, ofile);
//        ofile.close();
    }
	if (0)
	{
		//(renderOpt, threshold) -> [(PSNR, SSIM)_1k, (PSNR, SSIM)_3k, (PSNR, SSIM)_5k, (PSNR, SSIM)_10k]
		// Linear, Nearest Sampling have an effect on the result
		// # of frames also have an effect
		//-----------------------------input---------------------------------------------------------//
		//int modelId = 0;
		int coarseResId = 3, oriResId = 5;
		string fmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][oriResId] + ".ply";
		string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
		string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
		int numFrames = 300;
		int numTargets = 4;

		// resolution_threshold_renderMode_psnr_ssim_table
		// resolution_threshold_renderMode_time_missratio_table
		ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
		reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
		reprojMT.setPath(1, 1, 2);

		//-------------------render option----------------------------------//
		bool leftPrimary = true;
		bool enableFlip = false;
		bool debug = false;
		bool measureQuality = true;
		reprojMT.updateQuality(measureQuality);

		//parameters
		int numCoarseModels = 6;
		float initThreshold = 0.0002;
		float thresholdStep = 0.0002;
		int numThresholds = 12;
		vector<int> renderOptions = { 0, 1, 2, 3, 4 };
		int numRenderOption = renderOptions.size();
		int numRows = numRenderOption * numThresholds;
		int numCols = 2 + numCoarseModels * 3;
		vector<vector<double>> qualityTable(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 3, 0.0));
		//        vector<vector<double>> qualityTable2(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 3, 0.0));
		for (int renderOptId = 0; renderOptId < numRenderOption; renderOptId++) {
			int renderOption = renderOptions[renderOptId];
			reprojMT.updateRenderOption(renderOption);
			for (int coarseModelId = 0; coarseModelId < numCoarseModels; coarseModelId++) {
				outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + "/";
				cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + ".ply";
				cout << "coarse model " << cmodelPath << endl;
				reprojMT.updateCoarseModel(cmodelPath);
				reprojMT.updateDirectory(outDir);
				for (int i = 0; i < numThresholds; i++) {
					float thresholdVal = initThreshold + i * thresholdStep;
					auto res = reprojMT.renderReprojMT(thresholdVal, leftPrimary, enableFlip, debug);
					qualityTable[renderOptId * numThresholds + i][0] = renderOption;
					qualityTable[renderOptId * numThresholds + i][1] = thresholdVal;
					qualityTable[renderOptId * numThresholds + i][2 + coarseModelId * 3] = res[0];
					qualityTable[renderOptId * numThresholds + i][3 + coarseModelId * 3] = res[1];
					qualityTable[renderOptId * numThresholds + i][4 + coarseModelId * 3] = res[2];

					//                    qualityTable2[renderOptId * numThresholds + i][0] = renderOption;
					//                    qualityTable2[renderOptId * numThresholds + i][1] = thresholdVal;
					//                    qualityTable2[renderOptId * numThresholds + i][2 + coarseModelId * 3] = res[6];
					//                    qualityTable2[renderOptId * numThresholds + i][3 + coarseModelId * 3] = res[7];
					//                    qualityTable2[renderOptId * numThresholds + i][4 + coarseModelId * 3] = res[8];

					if (1) {
						cout << "result " << endl;
						for (auto v : res) {
							cout << v << " ";
						}
						cout << endl;
					}
				}
			}
		}

		string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption_ratio.csv";
		cout << "save excel " << ofileName << endl;
		ofstream ofile(ofileName);
		write_csv(qualityTable, numRows, numCols, ofile);
		ofile.close();

		//        ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption_time.csv";
		//        cout << "saving excel "<<ofileName << endl;
		//        ofile.open(ofileName);
		//        write_csv(qualityTable2, numRows, numCols_time, ofile);
		//        ofile.close();
	}
	if (0)
	{
		//(renderOpt, threshold) -> [(PSNR, SSIM)_1k, (PSNR, SSIM)_3k, (PSNR, SSIM)_5k, (PSNR, SSIM)_10k]
		// Linear, Nearest Sampling have an effect on the result
		// # of frames also have an effect
		//-----------------------------input---------------------------------------------------------//
		//int modelId = 0;
		int coarseResId = 3, oriResId = 5;
		string fmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][oriResId] + ".ply";
		string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
		string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
		int numFrames = 300;
		int numTargets = 4;

		// resolution_threshold_renderMode_psnr_ssim_table
		// resolution_threshold_renderMode_time_missratio_table
		ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
		reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
		reprojMT.setPath(1, 1, 2);
		reprojMT.setPath3(1, 1, 2, 200, 60, numFrames);// comment this if do not want a faster version

													   //-------------------render option----------------------------------//
		bool leftPrimary = true;
		bool enableFlip = false;
		bool debug = false;
		bool measureQuality = true;
		reprojMT.updateQuality(measureQuality);

		//parameters
		int numCoarseModels = 6;
		float initThreshold = 0.0002;
		float thresholdStep = 0.0002;
		int numThresholds = 12;
		vector<int> renderOptions = { 0, 1, 2, 3, 4 };
		int numRenderOption = renderOptions.size();
		int numRows = numRenderOption * numThresholds;
		int numCols = 2 + numCoarseModels * 2;
		vector<vector<double>> qualityTable(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 2, 0.0));
		//        vector<vector<double>> qualityTable2(numRenderOption * numThresholds, vector<double>(2 + numCoarseModels * 3, 0.0));
		for (int renderOptId = 0; renderOptId < numRenderOption; renderOptId++) {
			int renderOption = renderOptions[renderOptId];
			reprojMT.updateRenderOption(renderOption);
			for (int coarseModelId = 0; coarseModelId < numCoarseModels; coarseModelId++) {
				outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + "/";
				cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseModelId] + ".ply";
				cout << "coarse model " << cmodelPath << endl;
				reprojMT.updateCoarseModel(cmodelPath);
				reprojMT.updateDirectory(outDir);
				for (int i = 0; i < numThresholds; i++) {
					float thresholdVal = initThreshold + i * thresholdStep;
					auto res = reprojMT.renderReprojMT(thresholdVal, leftPrimary, enableFlip, debug);
					qualityTable[renderOptId * numThresholds + i][0] = renderOption;
					qualityTable[renderOptId * numThresholds + i][1] = thresholdVal;
					qualityTable[renderOptId * numThresholds + i][2 + coarseModelId * 2] = res[0];
					qualityTable[renderOptId * numThresholds + i][3 + coarseModelId * 2] = res[1];

					//                    qualityTable2[renderOptId * numThresholds + i][0] = renderOption;
					//                    qualityTable2[renderOptId * numThresholds + i][1] = thresholdVal;
					//                    qualityTable2[renderOptId * numThresholds + i][2 + coarseModelId * 3] = res[6];
					//                    qualityTable2[renderOptId * numThresholds + i][3 + coarseModelId * 3] = res[7];
					//                    qualityTable2[renderOptId * numThresholds + i][4 + coarseModelId * 3] = res[8];

					if (1) {
						cout << "result " << endl;
						for (auto v : res) {
							cout << v << " ";
						}
						cout << endl;
					}
				}
			}
		}

		string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption_faster.csv";
		cout << "save excel " << ofileName << endl;
		ofstream ofile(ofileName);
		write_csv(qualityTable, numRows, numCols, ofile);
		ofile.close();

		//        ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELNAMES[modelId] + "_" + "model_threshold_renderOption_time.csv";
		//        cout << "saving excel "<<ofileName << endl;
		//        ofile.open(ofileName);
		//        write_csv(qualityTable2, numRows, numCols_time, ofile);
		//        ofile.close();
	}
    if(0)
    {
        // graph PSNR/SSIM vs(rotation, translate, rotOrder)->(PSNR, SSIM)
        //-----------------------------input---------------------------------------------------------//
        //int modelId = 0;
        int coarseResId = 2, oriResId = 5;
        string fmodelPath = MODELDIR + MODELNAMES[modelId] +"/"+ MODELS[modelId][oriResId]+".ply";
        string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
        string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
        int numFrames = 200;

        int numTargets = 3;
        ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
        reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
        reprojMT.setPath(1, 1, 2);

        //-------------------render option----------------------------------//
        float threshold = 0.0016;
        int renderOptId = 2;
        bool debug = true;
        bool enableFlip = false;
        bool measureQuality = true;
        reprojMT.updateQuality(measureQuality);
        reprojMT.updateRenderOption(renderOptId);

        //-------------------parameters----------------------------------//
        int scaleOption = 2;
        int rotateOption = 1;
        int translateXOption = 1;
        int scalePeriod = 150;
        vector<int> leftPrimaries{ true, false };
        vector<int> rotatePeriods = { 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300 };
//        vector<int> rotatePeriods = { 50, 60};
//        vector<int> scalePeriods = { 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 350, 400}; // period to move from near to far
        vector<int> translatePeriods = { 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300 };
//        vector<int> translatePeriods = { 50, 60};

        // render to csv
        int numRotTransSet = rotatePeriods.size() * translatePeriods.size();
        int numRows = leftPrimaries.size() * numRotTransSet;
        int numCols = 6;
        int count = 0;
        vector<vector<double>> qualityTable(numRows, vector<double>(numCols, 0.0));
        for (int leftPrimaryId = 0; leftPrimaryId < leftPrimaries.size(); leftPrimaryId++) {
            bool leftPrimary = leftPrimaries[leftPrimaryId];
            for (auto rotatePeriod : rotatePeriods) {
                for (auto translatePeriod : translatePeriods) {
                    cout<<"rotate "<<rotatePeriod<<" translate "<<translatePeriod<<endl;
                    reprojMT.setPath3(scaleOption, rotateOption, translateXOption,
                                      scalePeriod, rotatePeriod, translatePeriod);
                    auto res = reprojMT.renderReprojMT(threshold, leftPrimary, enableFlip, debug);
                    qualityTable[count][0] = renderOptId;
                    qualityTable[count][1] = leftPrimary;
                    qualityTable[count][2] = rotatePeriod;
                    qualityTable[count][3] = translatePeriod;
                    qualityTable[count][4] = res[0];
                    qualityTable[count][5] = res[1];
                    count ++;
                }
            }
        }
        string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "_rot1_scale2_trans1.csv";
        cout << ofileName << endl;
        ofstream ofile(ofileName);
        write_csv(qualityTable, numRows, numCols, ofile);
        ofile.close();
    }
    if(0)
    {
        // graph PSNR/SSIM vs(rotation), legend: s1t1, t1s1
        // Shader usage: vs"reprojMultiTargetMultiOption.vs" fs"reprojMultiTargetMultiOption.fs"
        //-----------------------------input---------------------------------------------------------//
        //int modelId = 0;
        int coarseResId = 2, oriResId = 5;
        string fmodelPath = MODELDIR + MODELNAMES[modelId] +"/"+ MODELS[modelId][oriResId]+".ply";
        string cmodelPath = MODELDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + ".ply";
        string outDir = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "/";
        int numFrames = 10;

        int numTargets = 3;
        ReprojMT reprojMT(WINDOWWIDTH, WINDOWHEIGHT);
        reprojMT.init(fmodelPath, cmodelPath, numTargets, numFrames, outDir);
//        reprojMT.setPath(1, 1, 2);

        //-------------------render option----------------------------------//
        float threshold = 0.0016;
        bool debug = true;
        bool enableFlip = false;
        bool measureQuality = true;
        reprojMT.updateQuality(measureQuality);


        //-------------------parameters----------------------------------//
        int scaleOption = 2;
        int scalePeriod = 150;
        int rotateOption = 1;
        int translateXOption = 2;
        int translatePeriod = 120;
        bool leftPrimary = true;
        vector<int> rotatePeriods = { 20, 40, 60, 80, 100, 200, 300, 400, 500, 600};
        vector<int> renderOptions = {2, 5};
        int numRows = renderOptions.size() * rotatePeriods.size();
        int numCols = 4;
        vector<vector<double>> qualityTable(numRows, vector<double>(numCols, 0.0));
        int count = 0;
        for(int renderOptId =0; renderOptId < renderOptions.size(); renderOptId ++){
            int renderOpt = renderOptions[renderOptId];
            reprojMT.updateRenderOption(renderOpt);
            for (auto rotatePeriod : rotatePeriods) {
                reprojMT.updateNumFrames(rotatePeriod);
                reprojMT.setPath3(scaleOption, rotateOption, translateXOption,
                                  scalePeriod, rotatePeriod, translatePeriod);
                auto res = reprojMT.renderReprojMT(threshold, leftPrimary, enableFlip, debug);
                qualityTable[count][0] = renderOptId;
                qualityTable[count][1] = rotatePeriod;
                qualityTable[count][2] = res[0];
                qualityTable[count][3] = res[1];
                count ++;
            }
        }
        string ofileName = RESULTDIR + MODELNAMES[modelId] + "/" + MODELS[modelId][coarseResId] + "_rot1_"+ to_string(renderOptions[0]) + to_string(renderOptions[1]) +"_2.csv";
        cout << ofileName << endl;
        ofstream ofile(ofileName);
        write_csv(qualityTable, numRows, numCols, ofile);
        ofile.close();
    }

    return 1;
}

// TODO
// Checked image quality
// Checked saved texture float
// To check discard, re-shading, caching
// To check quality
