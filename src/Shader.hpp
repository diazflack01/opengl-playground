#pragma once

#include <string>
#include <glm/matrix.hpp>

class Shader {
public:
    Shader(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath);

    void use();

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setUInt(const std::string& name, unsigned value);
    void setFloat(const std::string& name, float value);
    void setVec2Float(const std::string& name, float v1, float v2);
    void setVec3Float(const std::string& name, float v1, float v2, float v3);
    void setMat4(const std::string& name, const glm::mat4 v);

    int getId();

private:
    void checkCompileErrors(unsigned int shader, std::string type);

    unsigned int mID;
};
