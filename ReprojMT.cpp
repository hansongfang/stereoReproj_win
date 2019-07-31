#include "ReprojMT.h"
#include <math.h>
#include <glm/gtc/matrix_inverse.hpp>
#define PI 3.14159265

// render -> cached1, render -> cached2, render -> cached3
// render -> cached -> cached -> cached -> update

extern string SHADERPATH;

ReprojMT::ReprojMT(int width, int height)
    :windowWidth(width)
    ,windowHeight(height)
    ,quadVAO(0)
    ,_renderOption(1)
    ,_measureQuality(true)
	,_theta(0.0)
	,_phi(0.0)
{
    _window = new GLWindow(windowWidth, windowHeight);
}

ReprojMT::~ReprojMT()
{
    delete _window;
    delete _camera;
    delete _a;
    delete _coarseA;
}

void ReprojMT::init(string fMeshPath, string cMeshPath, int numTargets, int numFrames,string directory)
{
    _directory = directory;

    _numTargets = numTargets;

    _numFrames = numFrames;

    // set model path
    _a = new Mesh(fMeshPath);
    _coarseA = new Mesh(cMeshPath);

    // Camera & stereo camera
    _camera = new Camera(glm::vec3(0.0, 0.0f, 0.8f));
    float IOD = 0.05;
    _stereoCam = new StereoCamera;
    _stereoCam->update(*_camera, windowWidth, windowHeight, IOD);

    // shader---------------------------------------------------------
    // quad shader for deferred rendering
    setupQuadShader();
    setupPhongShader();
    setupNoiseShader();
    setupReprojMTShader();

    // setup framebuffer and matrixes for target
    setupTargets(numTargets);
    setupRenderFBO();
    setupCacheFBO();

    // Opengl Global setting
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // very far position
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    // atomicbuffer
    this->setAtomicCounterbuffer();
}

void ReprojMT::updateQuality(bool quality)
{
    _measureQuality = quality;
}

void ReprojMT::updateDirectory(string directory)
{
    _directory = directory;
}

void ReprojMT::updateModels(string fMeshPath, string cMeshPath)
{
    delete _a;
    _a = new Mesh(fMeshPath);
    updateCoarseModel(cMeshPath);
}

void ReprojMT::updateCoarseModel(string cMeshPath) {
    delete _coarseA;
    _coarseA = new Mesh(cMeshPath);
}

// -------------------------SHADER----------------------------------//
void ReprojMT::setupQuadShader()
{
    string verPath = SHADERPATH + "quadshader.vs";
    string fragPath = SHADERPATH + "quadshader.fs";
    _quadShader.init(verPath.c_str(), fragPath.c_str());
}

void ReprojMT::updateQuadShader(GLuint gColor)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _quadShader.use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gColor);
    _quadShader.setInt("screenTexture", 1);
    renderQuad();
    _window->aftRender();
}

void ReprojMT::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void ReprojMT::setupPhongShader()
{
    string verPath = SHADERPATH + "phong.vs";
    string fragPath = SHADERPATH + "phong.fs";
    _phongShader.init(verPath.c_str(), fragPath.c_str());
    // set up material
    glm::vec3 materialColor(1.0, 1.0, 1.0);
    glm::vec3 ambientLightColor(0.6, 0.6, 0.6);
    glm::vec3 dirLightPos(320, 390, 700);
    glm::normalize(dirLightPos);
    glm::vec3 dirLightColor(0.33, 0.33, 0.33);

    _phongShader.use();
    _phongShader.setVec3("lightPos", &dirLightPos[0]);
    _phongShader.setVec3("uDirLightColor", &dirLightColor[0]);
    _phongShader.setVec3("uAmbientLightColor", &ambientLightColor[0]);
    _phongShader.setVec3("uMaterialColor", &materialColor[0]);
    _phongShader.setFloat("uKd", 0.7f);

    //loop
    _phongShader.SetUnInt("loopx", 1);
    _phongShader.SetUnInt("loopy", 1);
}

void ReprojMT::updatePhongShader(const worldViewProj& mvp)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _phongShader.use();
    auto fullTransform = mvp.Projection * mvp.worldView * mvp.modelWorld;
    _phongShader.setMat4("curFullTransform", (float*)&fullTransform[0][0]);
    auto modelView = mvp.worldView * mvp.modelWorld;
    _phongShader.setMat4("curModelView", (float*)&modelView[0][0]);
    auto invTransModelView = glm::inverseTranspose(modelView);
    _phongShader.setMat4("invTransModelView", (float*)&invTransModelView[0][0]);
    _phongShader.setMat4("curWorldView", (float*) &(mvp.worldView[0][0]));
}

void ReprojMT::setupNoiseShader()
{
    string verPath = SHADERPATH + "noise.vs";
    string fragPath = SHADERPATH + "noise.fs";
    _noiseShader.init(verPath.c_str(), fragPath.c_str());
    // set up material
    glm::vec3 materialColor(1.0, 1.0, 1.0);
    glm::vec3 ambientLightColor(0.6, 0.6, 0.6);
    glm::vec3 dirLightPos(320, 390, 700);
    glm::normalize(dirLightPos);
    glm::vec3 dirLightColor(0.33, 0.33, 0.33);

    _noiseShader.use();
    _noiseShader.setVec3("lightPos", &dirLightPos[0]);
    _noiseShader.setVec3("uDirLightColor", &dirLightColor[0]);
    _noiseShader.setVec3("uAmbientLightColor", &ambientLightColor[0]);
    _noiseShader.setVec3("uMaterialColor", &materialColor[0]);
    _noiseShader.setFloat("uKd", 0.7f);

    //loop
    _noiseShader.SetUnInt("loopx", 1);
    _noiseShader.SetUnInt("loopy", 1);
}

void ReprojMT::updateNoiseShader(const worldViewProj& mvp)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _noiseShader.use();
    auto fullTransform = mvp.Projection * mvp.worldView * mvp.modelWorld;
    _noiseShader.setMat4("curFullTransform", (float*)&fullTransform[0][0]);
    auto modelView = mvp.worldView * mvp.modelWorld;
    _noiseShader.setMat4("curModelView", (float*)&modelView[0][0]);
    auto invTransModelView = glm::inverseTranspose(modelView);
    _noiseShader.setMat4("invTransModelView", (float*)&invTransModelView[0][0]);
    _noiseShader.setMat4("curWorldView", (float*) &(mvp.worldView[0][0]));
}

void ReprojMT::setupReprojMTShader()
{
    string verPath = SHADERPATH + "reprojMultiTargetMultiOption.vs";
    string fragPath = SHADERPATH + "reprojMultiTargetMultiOption.fs";
    _reprojShaderMT.init(verPath.c_str(), fragPath.c_str());
    _reprojShaderMT.use();
    _reprojShaderMT.SetUnInt("loopx", 1);

    // reshading
    glm::vec3 materialColor(1.0, 1.0, 1.0);
    glm::vec3 ambientLightColor(0.6, 0.6, 0.6);
    glm::vec3 dirLightPos(320, 390, 700);
    glm::normalize(dirLightPos);
    glm::vec3 dirLightColor(0.33, 0.33, 0.33);
    glm::vec3 specularColor(1.0, 1.0, 1.0);

    _reprojShaderMT.use();
    _reprojShaderMT.setVec3("lightPos", &dirLightPos[0]);
    _reprojShaderMT.setVec3("uDirLightColor", &dirLightColor[0]);
    _reprojShaderMT.setVec3("uAmbientLightColor", &ambientLightColor[0]);
    _reprojShaderMT.setVec3("uMaterialColor", &materialColor[0]);
    _reprojShaderMT.setVec3("uSpecularColor", &specularColor[0]);
    _reprojShaderMT.setFloat("uKd", 0.7f);
}

// -------------------------FRAMEBUFFER----------------------------------//
void ReprojMT::setupTargets(int numTargets)
{
    _targetFBOS.clear();
    _targetFBOS.resize(numTargets, TargetFBO{ 0, 0, 0 });
    for (int i = 0; i < numTargets; i++) {
        cout << " setup target " << i << " fbo" << endl;
        setupFBO(_targetFBOS[i].gBuffer, _targetFBOS[i].gColor, _targetFBOS[i].gDepth);
    }

    // setup matrices
    _targetMVPS.clear();
    _targetMVPS.resize(numTargets);
    for(int i=0; i<numTargets; i++){
         _targetMVPS[i].modelWorld = glm::mat4(1.0);
    }
}

void ReprojMT::setupRenderFBO()
{
    setupFBO(_renderFBO.gBuffer, _renderFBO.gColor, _renderFBO.gDepth);
    _renderMVP.modelWorld = glm::mat4(1.0);
    _renderMVP.worldView = glm::mat4(1.0);
    _renderMVP.Projection = glm::mat4(1.0);
}

void ReprojMT::setupCacheFBO()
{
    setupFBO(_cacheFBO.gBuffer, _cacheFBO.gColor, _cacheFBO.gDepth);
    _cacheMVP.modelWorld = glm::mat4(1.0);
    _cacheMVP.worldView = glm::mat4(1.0);
    _cacheMVP.Projection = glm::mat4(1.0);
}

void ReprojMT::setupFBO(GLuint& gBuffer, GLuint& gColor, GLuint& gDepth)
{
    cout<<"setting framebuffer"<<endl;
    // configure g-buffer framebuffer
    // ------------------------------
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    /*cout << "window width " << windowWidth << endl;
    cout << "window Height " << windowHeight << endl;*/
    // color buffer
    glGenTextures(1, &gColor);
    glBindTexture(GL_TEXTURE_2D, gColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColor, 0);
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // depth buffer
    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    else
        cout << "Frame buffer complete \n " << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// -------------------------Option----------------------------------//
void ReprojMT::updateRenderOption(int renderOption) {
    _reprojShaderMT.use();
    _reprojShaderMT.setInt("renderOption", renderOption);
    _renderOption = renderOption;
}

void ReprojMT::updateNumFrames(int numFrames)
{
    _numFrames = numFrames;
    cout << "update path" << endl;
}

void ReprojMT::updateShaderComplex(int loopVS, int loopFS)
{
    //loop
    _phongShader.use();
    _phongShader.SetUnInt("loopx", loopVS);
    _phongShader.SetUnInt("loopy", loopFS);

    _reprojShaderMT.use();
    _reprojShaderMT.SetUnInt("loopx", loopVS);
}

// -------------------------Reprojection----------------------------------//
void ReprojMT::updateReprojMTShader(int numTarget, const vector<int>& targetIds, worldViewProj& curMVP)
{
    _reprojShaderMT.use();

    // current matrix
    auto fullTransform = curMVP.Projection * curMVP.worldView * curMVP.modelWorld;
    _reprojShaderMT.setMat4("curFullTransform", (float*)&fullTransform[0][0]);
    auto modelView = curMVP.worldView * curMVP.modelWorld;
    _reprojShaderMT.setMat4("curModelView", (float*)&modelView[0][0]);
    auto invTransModelView = glm::inverseTranspose(modelView);
    _reprojShaderMT.setMat4("invTransModelView", (float*)&invTransModelView[0][0]);
    _reprojShaderMT.setMat4("curWorldView", (float*) &(curMVP.worldView[0][0]));

    int count = 0;
    string temp;
    for (int i = 0; i < numTarget; i++) {
        int targetId = targetIds[i];
        auto fullTransform = _targetMVPS[targetId].Projection * _targetMVPS[targetId].worldView * _targetMVPS[targetId].modelWorld;
        temp = "cacheFullTransform[" + to_string(i) + "]";
        _reprojShaderMT.setMat4(temp, (float*)&fullTransform[0][0]);

        // set textures
        glActiveTexture(GL_TEXTURE0+count);
        glBindTexture(GL_TEXTURE_2D, _targetFBOS[targetId].gColor);
        temp = "tts[" + to_string(i) + "].tDiffuse";
        _reprojShaderMT.setInt(temp, count);
        count = count + 1;

        glActiveTexture(GL_TEXTURE0 + count);
        glBindTexture(GL_TEXTURE_2D, _targetFBOS[targetId].gDepth);
        temp = "tts[" + to_string(i) + "].tDepth";
        _reprojShaderMT.setInt(temp, count);
        count = count + 1;
    }
}

vector<float> ReprojMT::renderReprojMT(float threshold, bool leftPrimary, bool enableFlip, bool debug)
{
    bool savePNG = true;
    bool usePhong = true;
    _measureQuality = true;
    float thresholdId = threshold / 0.0001;
    string thresholdDir = _directory + to_string(int(round(thresholdId))) + "/";
    cout <<"save result in "<< thresholdDir << endl;
    cout<<"use render option "<<_renderOption<<endl;

    _reprojShaderMT.use();
    _reprojShaderMT.setFloat("threshold", threshold);
    _reprojShaderMT.setFloat("epsilon", -0.0002);
    _reprojShaderMT.setBool("usePhong", usePhong);

    worldViewProj curMVP;
    curMVP.modelWorld = glm::mat4(1.0);
    curMVP.worldView = glm::mat4(1.0);
    curMVP.Projection = glm::mat4(1.0);

    bool renderLeft = leftPrimary;
    for(int frameId =0; frameId < _numTargets-1; frameId++){
        int curId = frameId;
        setMVP(frameId, renderLeft, _targetMVPS[curId]);
        glBindFramebuffer(GL_FRAMEBUFFER, _targetFBOS[curId].gBuffer);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(usePhong){
            updatePhongShader(_targetMVPS[curId]);
            _a->Draw(_phongShader);
        }
        else{
            updateNoiseShader(_targetMVPS[curId]);
            _a->Draw(_noiseShader);
        }
        if (debug) {
            this->saveFigure(savePNG, frameId, thresholdDir, renderLeft);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    float avgPSNR = 0.0, avgSSIM = 0.0;
	float avgMissRatio = 0.0;
    int countPSNR = 0;
    cv::Mat repMat, gdMat;
    vector<int> targetIds(_numTargets, 0); // track the reference images
    for(int frameId = _numTargets-1; frameId < _numFrames; frameId++){
        int curId = frameId % _numTargets;
        string temp;
        for (int i = 0; i < _numTargets; i++) {
            targetIds[i] = (frameId - i) % _numTargets;
            temp += to_string(targetIds[i])+" ";
        }

        setMVP(frameId, renderLeft, _targetMVPS[curId]);
        glBindFramebuffer(GL_FRAMEBUFFER, _targetFBOS[curId].gBuffer);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(usePhong){
            updatePhongShader(_targetMVPS[curId]);
            _a->Draw(_phongShader);
        }
        else{
            updateNoiseShader(_targetMVPS[curId]);
            _a->Draw(_noiseShader);
        }
        if (debug) {
            this->saveFigure(savePNG, frameId, thresholdDir, renderLeft);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        renderLeft = !renderLeft;

        // cache
        setMVP(frameId, renderLeft, curMVP);
        glBindFramebuffer(GL_FRAMEBUFFER, _cacheFBO.gBuffer);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        updateReprojMTShader(_numTargets, targetIds, curMVP);
		this->clearAtomicCounterbuffer();
		auto res = this->readAtomicCounterbuffer();
		_coarseA->Draw(_reprojShaderMT);
		res = this->readAtomicCounterbuffer();
		float tempMissRatio = (float)res.second / (float)res.first;
		avgMissRatio += tempMissRatio;
        //cout<<tempMissRatio<<endl;
        if (debug) {
            this->saveFigure(savePNG, frameId, thresholdDir, renderLeft);
        }
        auto reprojBuffer = _window->getFrameBuffer();
        repMat = cv::Mat(windowHeight, windowWidth, CV_8UC3, &reprojBuffer[0]);
        cv::flip(repMat, repMat, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (_measureQuality) {
//            cout<<"measure quality"<<endl;
            glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO.gBuffer);
			glViewport(0, 0, windowWidth, windowHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if(usePhong){
                updatePhongShader(curMVP);
                _a->Draw(_phongShader);
            }
            else{
                updateNoiseShader(curMVP);
                _a->Draw(_noiseShader);
            }
            //_window->aftRender();
            if(debug){
                this->saveGTFigure(savePNG, frameId, thresholdDir, renderLeft);
            }
            auto gdBuffer = _window->getFrameBuffer();
            gdMat = cv::Mat(windowHeight, windowWidth, CV_8UC3, &gdBuffer[0]);
            cv::flip(gdMat, gdMat, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            auto quality = _quality.computeQualityMat(repMat, gdMat);
//            cout<<"quality "<<quality.first<<" "<<quality.second<<endl;
            avgPSNR += quality.second;
            avgSSIM += quality.first;
            countPSNR += 1;
        }
        renderLeft = !renderLeft;
    }
    if(_measureQuality){
        avgPSNR /= countPSNR;
        avgSSIM /= countPSNR;
		avgMissRatio /= countPSNR;
        cout<<"psnr "<<avgPSNR<<" ssim "<<avgSSIM<<" missratio "<<avgMissRatio<<endl;
    }
    return vector<float>{avgPSNR, avgSSIM, avgMissRatio};
}

void ReprojMT::saveFigure(bool savePNG, int frameId, const string &thresholdDir, bool renderLeft)
{
    string filename = thresholdDir;
    if(renderLeft)
        filename += "left_frame" + to_string(frameId);
    else
        filename += "right_frame"+ to_string(frameId);
    if(savePNG)
        filename += ".png";
    else
        filename += ".jpg";
    //cout<<"save figure "<<filename<<endl;
    if(savePNG)
        _window->saveFramebuffer(filename, "png", PNG_DEFAULT);
    else
        _window->saveFramebuffer(filename, "jpg", JPEG_DEFAULT);
}

void ReprojMT::saveGTFigure(bool savePNG, int frameId, const string &thresholdDir, bool renderLeft)
{
    string filename = thresholdDir;
    if(renderLeft)
        filename += "gt_left_frame" + to_string(frameId);
    else
        filename += "gt_right_frame"+ to_string(frameId);
    if(savePNG)
        filename += ".png";
    else
        filename += ".jpg";
    //cout<<"save figure "<<filename<<endl;
    if(savePNG)
        _window->saveFramebuffer(filename, "png", PNG_DEFAULT);
    else
        _window->saveFramebuffer(filename, "jpg", JPEG_DEFAULT);
}


// -------------------------Path----------------------------------//
void ReprojMT::setMVP(int frameId, bool left, worldViewProj& mvp)
{
    float IOD = 0.05;

    _camera->Position.z = _cameraPath[frameId];
    _stereoCam->update(*_camera, windowWidth, windowHeight, IOD);
    mvp.worldView = _stereoCam->getView(left);
    mvp.Projection = _stereoCam->getProj(left);

    float curDegree = _modelRotateYPath[frameId];
    float translateX = _modelTranslateXPath[frameId];
	/*cout << "update pose " << endl;
	cout << translateX << endl;
	cout << curDegree << endl;
	getchar();*/
    glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(translateX, 0.0f, 0.0f));
    //mvp.modelWorld = glm::rotate(trans, glm::radians(curDegree), glm::vec3(0.0, 1.0, 0.0));
	/*float theta = 0 * PI / 180.0 ;
	float phi = 0 * PI / 180.0;*/
	float y = cos(_theta);
	float x = sin(_theta) * sin(_phi);
	float z = sin(_theta) * cos(_phi);
	//cout << "rotation vector " << glm::to_string(glm::vec3(x, y, z)) << endl;
	mvp.modelWorld = glm::rotate(trans, curDegree, glm::vec3(x, y, z));
}

// set path
void ReprojMT::setCameraPath(float initZ, float stepZ, int option)
{
    _cameraPath.clear();
    _cameraPath.resize(_numFrames, 0.0);

    if (option == 1) {
        for (int i = 0; i < _numFrames; i++) {
            _cameraPath[i] = initZ + i * stepZ;
        }
    }
    else if (option == 2) {
        for (int i = 0; i < _numFrames; i++) {
            _cameraPath[i] = initZ + 0.3;
        }
    }
    else if (option == 3) {
        for (int i = 0; i < _numFrames; i++) {
            _cameraPath[i] = initZ + (_numFrames - i - 1) * stepZ;
        }
    }
}

void ReprojMT::setModelRotateYPath(int rotatePeriod, int option)
{
    _modelRotateYPath.clear();
    _modelRotateYPath.resize(_numFrames, 0.0);

	float initRotateY = 180.0f;
    float stepRotateY = 360.0f / rotatePeriod;
    if (option == 1) {
        for (int i = 0; i < _numFrames; i++) {
            _modelRotateYPath[i] = i * stepRotateY + initRotateY;
        }
    }
    else if (option == 2) {
        for (int i = 0; i < _numFrames; i++) {
            _modelRotateYPath[i] = 0 * stepRotateY + initRotateY;
        }
    }
    else if (option == 3) {
        for (int i = 0; i < _numFrames; i++) {
            _modelRotateYPath[i] = -i * stepRotateY + initRotateY;
        }
    }
}

void ReprojMT::setModelTranslateXPath(float stepTranslateX, int option)
{
    _modelTranslateXPath.clear();
    _modelTranslateXPath.resize(_numFrames, 0.0f);

    if (option == 1) {
        for (int i = 0; i < _numFrames; i++) {
            _modelTranslateXPath[i] = (i - _numFrames / 2) * stepTranslateX;
        }
    }
    else if (option == 2) {
        for (int i = 0; i < _numFrames; i++) {
            _modelTranslateXPath[i] = 0.0;
        }
    }
    else if (option == 3) {
        for (int i = 0; i < _numFrames; i++) {
            _modelTranslateXPath[i] = -(i - _numFrames / 2) * stepTranslateX;
        }
    }
}

void ReprojMT::setPath(int cameraOption, int roateOption, int translateOption)
{
    // numFrames to move from near to far
    // which is near and which is far
    // 50 - 400
    float initCameraZ = 1.0;
    if (translateOption != 2) {
        initCameraZ = 1.3;
        cout << "init camera " << initCameraZ << endl;
    }
    int periodCamera = 200;
    float stepCameraZ = 0.6 / periodCamera;
    setCameraPath(initCameraZ, stepCameraZ, cameraOption);

    // 100 - 360
    // degree/second = degree/frame * frame/second = 360/period * frame/second
    int rotatePeriod = 150;
    setModelRotateYPath(rotatePeriod, roateOption);

    // numFrames to move from left to right
    //int translateXPeriod = 100;
    // 50 - 300
    float stepTranslateX = 0.6/ _numFrames; //period = 100,
    setModelTranslateXPath(stepTranslateX, translateOption);
}

void ReprojMT::setPath3(int cameraOption, int rotateOption, int translateOption,
                        int scalePeriod, int rotatePeriod, int translateXPeriod)
{
    // numFrames to move from near to far
    // 50 - 400
    float initCameraZ = 1.0;
    if (translateOption != 2) {
        this->updateNumFrames(translateXPeriod);
        initCameraZ = 1.3;
        cout << "init camera " << initCameraZ << endl;
    }
    float stepCameraZ = 0.6 / scalePeriod;
    setCameraPath(initCameraZ, stepCameraZ, cameraOption);

    // degree/second = degree/frame * frame/second = 360/period * frame/second
    // 100 - 360
    setModelRotateYPath(rotatePeriod, rotateOption);


    // numFrames to move from left to right
    // 50 - 300
    float stepTranslateX = 0.6 / translateXPeriod; //period = 100,
    setModelTranslateXPath(stepTranslateX, translateOption);
}

// -------------------------AtomicCounterBuffer----------------------------------//
// atomic counter buffer
void ReprojMT::setAtomicCounterbuffer()
{
    cout<<"setting atomic buffer"<<endl;
    glGenBuffers(1, &_atomicBufferID);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _atomicBufferID);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint)*2, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _atomicBufferID);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    cout<<"finish setting atomic buffer\n"<<endl;
}

void ReprojMT::clearAtomicCounterbuffer()
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _atomicBufferID);
    GLuint a[2] = {0, 0};
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint)*2, &a);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

pair<GLuint, GLuint> ReprojMT::readAtomicCounterbuffer()
{
    // cout<<"read counter buffer"<<endl;
    GLuint userCounter[2];
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _atomicBufferID);
    glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint)*2, &userCounter);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    //printf("redPixels: %u %u\n", userCounter[0], userCounter[1]);
    return std::make_pair(userCounter[0], userCounter[1]);
}
