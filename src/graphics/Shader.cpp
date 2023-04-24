#include "Shader.hpp"

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

enum class ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Size = 3
};

std::string toStr(ShaderType shaderType) {
    switch (shaderType) {
        case ShaderType::Vertex:
            return "VERTEX";
        case ShaderType::Fragment:
            return "FRAGMENT";
        case ShaderType::Geometry:
            return "GEOMETRY";
        default:
            std::cerr << "Invalid ShaderType: " << static_cast<size_t>(shaderType) << std::endl;
            abort();
    }
}

Shader::Shader(const char *vertexShaderSourcePath, const char *fragmentShaderSourcePath, std::optional<const char*> geometryShaderSourcePath) {
    std::cout << "Compiling shaders: \n";
    std::cout << " Vertex file: " << vertexShaderSourcePath << "\n";
    std::cout << " Fragment file: " << fragmentShaderSourcePath << "\n";
    if (geometryShaderSourcePath.has_value()) {
        std::cout << " Geometry file: " << geometryShaderSourcePath.value() << "\n";
    }

    // 1. retrieve the shader source code from filePath
    auto parseTextFile = [](const char* file) -> std::optional<std::string> {
        std::ifstream ifStream;
        // ensure ifstream objects can throw exceptions:
        ifStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            ifStream.open(file);
            // read file buffer contents into stream
            std::stringstream ss;
            ss << ifStream.rdbuf();
            // close file handler
            ifStream.close();

            return ss.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << file << " ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;

            return std::nullopt;
        }
    };

    // 2. compile shaders
    auto compileShaderSource = [&](const std::string& shaderSource, ShaderType shaderType){
        const unsigned shaderId = glCreateShader(static_cast<size_t>(shaderType));
        const char* shaderSrcCStr = shaderSource.c_str();
        glShaderSource(shaderId, 1, &shaderSrcCStr, NULL);
        glCompileShader(shaderId);
        checkCompileErrors(shaderId, toStr(shaderType));
        return shaderId;
    };

    // yet another helper lambda
    auto parseAndCompileShader = [&](const char* file, ShaderType shaderType){
        const std::string shaderCode = *parseTextFile(file);
        const char* shaderCodeCStr = shaderCode.c_str();
        const unsigned compiledShaderId = compileShaderSource(shaderCodeCStr, shaderType);
        return compiledShaderId;
    };

    const unsigned vertexShaderId = parseAndCompileShader(vertexShaderSourcePath, ShaderType::Vertex);
    const unsigned fragmentShaderId = parseAndCompileShader(fragmentShaderSourcePath, ShaderType::Fragment);

    // shader Program
    mID = glCreateProgram();
    glAttachShader(mID, vertexShaderId);
    glAttachShader(mID, fragmentShaderId);

    if (geometryShaderSourcePath.has_value()) {
        const unsigned geometryShaderId = compileShaderSource(geometryShaderSourcePath.value(), ShaderType::Geometry);
        glAttachShader(mID, geometryShaderId);
        

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        glDeleteShader(geometryShaderId);
        return;
    } else {
        glLinkProgram(mID);
        checkCompileErrors(mID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
    }
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

void Shader::setVec2(const std::string& name, const glm::vec2& v) {
    setVec2Float(name, v.x, v.y);
}
