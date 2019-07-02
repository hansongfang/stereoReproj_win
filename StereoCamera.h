#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class StereoCamera
{
public:
    StereoCamera();
    ~StereoCamera();
    void update(const Camera& cam, int width, int height, float IOD);
    glm::mat4 getView(bool left_eye);
    glm::mat4 getProj(bool left_eye);

    float getSeparation() { return _separation; }
    float getNearZ() { return nearZ; }
    float getNormX() { return _right - _left; }
private:
    // central camera info
    glm::vec3     c_position;
    float         fovy;
    float         nearZ;
    float         farZ;
    float         aspect_ratio;

    glm::mat4     projection_right;
    glm::mat4     projection_left;
    glm::mat4     view_right;
    glm::mat4     view_left;

    // for later compute parallax
    float         _left;
    float         _right;
    float         _separation;

    void computeStereoViewProjection(float IOD, bool left_eye);
};

#endif
