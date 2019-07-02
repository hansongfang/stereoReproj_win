#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    // get the filepath, read the shader code, shader source, compile shader
    Shader() {}

    void init(const char* vertexPath, const char* fragmentPath, const char* geoPath = 0)
    {
        std::cout<<"loading shader"<<std::endl;
        std::cout<<vertexPath<<std::endl;
        std::cout<<fragmentPath<<std::endl;

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode = loadFile(vertexPath);
        std::string fragmentCode = loadFile(fragmentPath);
        std::string geoCode;
        if(geoPath)
            geoCode = loadFile(geoPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geoCode.c_str();

        // 2. compile shaders
        GLuint vertex = initshaders(GL_VERTEX_SHADER, vShaderCode);
        GLuint fragment = initshaders(GL_FRAGMENT_SHADER, fShaderCode);
        GLuint geometry = initshaders(GL_GEOMETRY_SHADER, gShaderCode);

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if(geoPath)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);

        std::cout<<"finish building shader\n "<<std::endl;
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetUnInt(const std::string &name, GLuint value) const
    {
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, float* value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, (float*)value);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, float* value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, (float*)value);
    }

    void setVec3(const std::string &name, float* value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, (float*) value);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    GLuint initshaders(GLenum type, const char* shaderCode) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
        if (type == GL_VERTEX_SHADER)
            checkCompileErrors(shader, "VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            checkCompileErrors(shader, "FRAGMENT");
        else if (type == GL_GEOMETRY_SHADER)
            checkCompileErrors(shader, "GEOMETRY");
        else
            std::cout << "not available shader type" << type << std::endl;
        return shader;
    }

    std::string loadFile(const char* filename)
    {
        std::string shaderCode;
        std::ifstream shaderFile;
        // ensure ifstream objects can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            shaderFile.open(filename);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            shaderCode = vShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        return shaderCode;
    }
};
#endif
