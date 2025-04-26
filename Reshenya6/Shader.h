#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {
public:
    GLuint shaderProgramID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode = loadFile(vertexPath);
        std::string fragmentCode = loadFile(fragmentPath);

        const char* vertexShaderCode = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertexShader);
        glAttachShader(shaderProgramID, fragmentShader);
        glLinkProgram(shaderProgramID);

        checkProgramLinking(shaderProgramID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void use() {
        glUseProgram(shaderProgramID);
    }

    void setMat4(const std::string& name, const float* value) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, value);
    }
 

    void setVec3(const std::string& name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y, z);
    }

    void setFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
    }

private:
    std::string loadFile(const char* path) {
        std::ifstream shaderFile(path);
        std::stringstream shaderStream;

        if (!shaderFile.is_open()) {
            std::cerr << "Could not open file: " << path << std::endl;
            return "";
        }

        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    }

    GLuint compileShader(GLenum shaderType, const char* shaderCode) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCode, nullptr);
        glCompileShader(shader);

        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
        }

        return shader;
    }

    void checkProgramLinking(GLuint program) {
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);
            std::cerr << "Program Linking Error: " << infoLog << std::endl;
        }
    }
};

#endif
