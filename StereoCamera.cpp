#include "StereoCamera.h"



StereoCamera::StereoCamera()
    :_left(0.0f)
    ,_right(0.0f)
    ,_separation(0.0f)
{
}


StereoCamera::~StereoCamera()
{
}

glm::mat4 StereoCamera::getView(bool left_eye)
{
    if (left_eye)
        return view_left;
    else
        return view_right;
}

glm::mat4 StereoCamera::getProj(bool left_eye)
{
    if (left_eye)
        return projection_left;
    else
        return projection_right;
}

void StereoCamera::update(const Camera& cam, int width, int height, float IOD)
{
    c_position = cam.Position;
    fovy = glm::radians(cam.Zoom);
    nearZ = cam.near;
    farZ = cam.far;
    aspect_ratio = (float)width / (float)height;

    this->computeStereoViewProjection(IOD, false);
    this->computeStereoViewProjection(IOD, true);
}


// this computation is for camera with no rotation, if rotate, change camera position, up vector and target
void StereoCamera::computeStereoViewProjection(float IOD, bool left_eye)
{
    float depthZ = 1.0;
    // set separation
    _separation = IOD;
    // mirror for left eye
    float left_right_dir = -1.0f;
    if (left_eye)
        left_right_dir = 1.0f;

    // perpective matrix
    double frustumshift = (IOD / 2)*nearZ / depthZ;
    float top = tan(fovy / 2)*nearZ; // fov y,z or x,z
    float bottom = -top;
    float right = aspect_ratio * top + frustumshift * left_right_dir; // relative to eye space
    float left = -aspect_ratio * top + frustumshift * left_right_dir;
    auto proj_matrix = glm::frustum(left, right, bottom, top, nearZ, farZ);
    _left = left;
    _right = right;

    // view matrix
    glm::vec3 up = glm::vec3(0, 1.0, 0);
    glm::vec3 cameraPos = c_position - glm::vec3(left_right_dir*IOD / 2, 0, 0);
    auto view_matrix = glm::lookAt(cameraPos, cameraPos - glm::vec3(0.0, 0.0, 1.0), up);

    // set matrix
    if (left_eye) {
        projection_left = proj_matrix;
        view_left = view_matrix;
    }else {
        projection_right = proj_matrix;
        view_right = view_matrix;
    }
}
