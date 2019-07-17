#ifndef REPROJMT_H
#define REPROJMT_H

#include "GLWindow.h"
#include "Mesh.h"
#include "Camera.h"
#include "StereoCamera.h"
#include "ImageQuality.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <FreeImage.h>
#include "Mesh.h"

struct worldViewProj {
    glm::mat4 modelWorld;
    glm::mat4 worldView;
    glm::mat4 Projection;
};

struct TargetFBO {
    GLuint gBuffer;
    GLuint gColor;
    GLuint gDepth;
};

class ReprojMT
{
public:
    ReprojMT(int width, int height);
    ~ReprojMT();

    void init(string fMeshPath, string cMeshPath,int numTargets, int numFrames, string directory = "./cached");

    void setPath(int cameraOption, int roateOption, int translateOption);
    void setPath3(int cameraOption, int roateOption, int translateOption,
                 int scalePeriod, int rotatePeriod, int translateXPeriod);

    vector<float> renderReprojMT(float threshold, bool leftPrimary, bool enableFlip,
        bool debug = false);

    void updateModels(string fMeshPath, string cMeshPath);
    void updateCoarseModel(string cMeshPath);
    void updateShaderComplex(int loopVS, int loopFS);
    void updateDirectory(string directory);
    void updateRenderOption(int renderOption);
    void updateNumFrames(int numFrames);
    void updateQuality(bool quality);

	void updateThetaPhi(float theta, float phi) {
		_theta = theta;
		_phi = phi;
	}

private:
    string                 _directory;
    GLWindow*              _window;
    int                    windowWidth, windowHeight;
	Mesh*                  _a;
	Mesh*                  _coarseA;
    Camera*                _camera;
    StereoCamera*          _stereoCam;

    Shader                 _quadShader;
    Shader                 _phongShader;
    Shader                 _reprojShaderMT;
    Shader                 _noiseShader;


    int                    _renderOption;// use _renderOption  num targets
    int                    _numTargets;
    vector<worldViewProj>  _targetMVPS;
    vector<TargetFBO>      _targetFBOS;
    TargetFBO              _renderFBO;
    worldViewProj          _renderMVP;
    TargetFBO              _cacheFBO;
    worldViewProj              _cacheMVP;
    GLuint                 quadVAO, quadVBO;

    // path
    int                    _numFrames;
    vector<float>          _cameraPath;
    vector<float>          _modelRotateYPath;
    vector<float>          _modelTranslateXPath;
    bool                   _measureQuality;
    ImageQuality           _quality;
    GLuint                 _atomicBufferID;// atomic counter buffer

    // option
//    bool                   _useMask;
//    int                    _useSpecular;
	// rotation
	float                   _theta;
	float                   _phi;

    void setupPhongShader();
    void updatePhongShader(const worldViewProj& mvp);

    void setupNoiseShader();
    void updateNoiseShader(const worldViewProj& mvp);

    void setupQuadShader();
    void updateQuadShader(GLuint gColor);
    void renderQuad();

    void setupReprojMTShader();
    void updateReprojMTShader(int numTarget, const vector<int>& targetIds, worldViewProj& curMVP);
    //void updateReprojMTShader(const worldViewProj& curMVP);

    // target framebuffer
    void setupTargets(int numTargets);
    void setupRenderFBO();
    void setupCacheFBO();
    void setupFBO(GLuint& gBuffer, GLuint& gColor, GLuint& gDepth);

    // path
    void setMVP(int frameId, bool left, worldViewProj& mvp);
    void setCameraPath(float initZ, float stepZ, int option = 1);
    void setModelRotateYPath(int periodMotion, int option = 1);
    void setModelTranslateXPath(float stepTranslateX, int option = 1);

    // Atomic counter buffer
    void setAtomicCounterbuffer();
    void clearAtomicCounterbuffer();
    pair<GLuint, GLuint> readAtomicCounterbuffer();

    void saveFigure(bool savePNG, int frameId, const string& thresholdDir, bool renderLeft);
    void saveGTFigure(bool savePNG, int frameId, const string& thresholdDir, bool renderLeft);

};

#endif // REPROJMT_H
