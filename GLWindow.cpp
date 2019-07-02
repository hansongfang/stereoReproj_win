#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <FreeImage.h>
#include "GLWindow.h"
#define GLM_ENABLE_EXPERIMENTAL

using namespace glm;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLWindow::GLWindow(int src_width, int src_height)
    :_width(src_width)
    ,_height(src_height)
{
    // glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(src_width, src_height, "test", NULL, NULL);
    if (_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return ;
    }
    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

    // load all OpenGL functions
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

GLWindow::~GLWindow()
{
}


void GLWindow::preRender()
{
    // input
    // -----
    //processInput(_window);

    // render
    // change render function
    // ------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void GLWindow::aftRender()
{
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void GLWindow::terminate()
{

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

vector<GLubyte> GLWindow::getFrameBuffer()
{
    vector<GLubyte> pixelBuffer(_width*_height *3);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, _width, _height, GL_BGR, GL_UNSIGNED_BYTE, &pixelBuffer[0]);
    return pixelBuffer;
}

void GLWindow::saveFramebuffer(string filename, string imageEXT, int flag)
{
    GLubyte* pixelBuffer = (GLubyte*)malloc(_width*_height * 3);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, _width, _height, GL_BGR, GL_UNSIGNED_BYTE, pixelBuffer);
    FIBITMAP* image = FreeImage_ConvertFromRawBits(pixelBuffer, _width, _height, 3 * _width,
        24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    if (imageEXT == "png")
        FreeImage_Save(FIF_PNG, image, filename.c_str(), flag);
    else if (imageEXT == "jpg")
        FreeImage_Save(FIF_JPEG, image, filename.c_str(), flag);
    FreeImage_Unload(image);
    delete[] pixelBuffer;
}

//void GLWindow::createQuad()
//{
//    // Quad geometry
//    vec3 verts[] = { vec3(-1, -1, 0), vec3{1, -1, 0}, vec3{-1, 1, 0}, vec3{1, 1, 0} };
//    GLushort indices[] = { 0, 1, 2, 3 };

//    // Set VAO, VBO, IBO
//    glGenVertexArrays(1, &_vaoQuad);
//    glGenBuffers(1, &_vboQuad);
//    glGenBuffers(1, &_iboQuad);
//    // bind vertex array object first, then bind and set vertex buffers and then configure vertex attributes
//    glBindVertexArray(_vaoQuad);

//    //Allocate vbos for data
//    //Upload vertex data
//    glBindBuffer(GL_ARRAY_BUFFER, _vboQuad);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
//    glEnableVertexAttribArray(0);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboQuad);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//    //Unplug Vertex Array
//    glBindVertexArray(0);

//    GLenum err = glGetError();

//    // Use shader
//    std::string verPath = ShaderPath + "quadshader.vs";
//    std::string fragPath = ShaderPath + "quadshader.fs";
//    _quadShader.init(verPath.c_str(), fragPath.c_str());
//}

//void GLWindow::drawQuad()
//{
//    _quadShader.use();
//    glBindVertexArray(_vaoQuad);
//    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//}

