#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Mesh.h"
using namespace std;

// set up window
// set up function interacting with window
class GLWindow
{
private:
    GLFWwindow*    _window;
    int            _width, _height;
public:
    GLWindow(int src_width, int src_height);
    ~GLWindow();
    void preRender();
    void aftRender();
    void terminate();
    void saveFramebuffer(string filename, string imageEXT="png", int flat=0);
    vector<GLubyte> getFrameBuffer();

//    draw quad
//    GLuint         _vaoQuad, _vboQuad, _iboQuad;
//    Shader         _quadShader;
//    Shader         _meshShader;
//    void createQuad();
//    void drawQuad();

};

#endif
