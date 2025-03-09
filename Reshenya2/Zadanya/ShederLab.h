#pragma once
#ifndef SHEDERLAB_H
#define SHEDERLAB_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

class Shader {
public:
    GLuint Program;

    Shader(const string& vertexPath, const string& fragmentPath);
    void Use();
    void SetUniform(const string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void SetUniform(const string& name, GLfloat v0, GLfloat v1, GLfloat v2);
    void SetUniform(const string& name, GLfloat v0, GLfloat v1);
    void SetUniform(const string& name, GLfloat v0);
    void SetUniform(const string& name, GLint v0);

private:
    void checkCompileErrors(GLuint shader, const string& type);
};

#endif