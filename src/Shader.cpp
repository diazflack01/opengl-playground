#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const char *vertexShaderSourcePath, const char *fragmentShaderSourcePath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexShaderSourcePath);
        fShaderFile.open(fragmentShaderSourcePath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    mID = glCreateProgram();
    glAttachShader(mID, vertex);
    glAttachShader(mID, fragment);
    glLinkProgram(mID);
    checkCompileErrors(mID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(mID);
}

void Shader::setBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(getId(), name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(getId(), name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(getId(), name.c_str()), value);
}

int Shader::getId() {
    return mID;
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Shader::setVec2Float(const std::string &name, float v1, float v2) {
    glUniform2f(glGetUniformLocation(getId(), name.c_str()), v1, v2);
}

void Shader::setMat4(const std::string &name, const glm::mat4 v) {
    glUniformMatrix4fv(glGetUniformLocation(getId(), name.c_str()), 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::setVec3Float(const std::string &name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(getId(), name.c_str()), v1, v2, v3);
}

void Shader::setUInt(const std::string &name, unsigned int value) {
    glUniform1ui(glGetUniformLocation(getId(), name.c_str()), value);
}
