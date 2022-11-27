#pragma once

#include <string>

class Shader {
public:
    Shader(const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath);

    void use();

    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2Float(const std::string& name, float v1, float v2);

    int getId();

private:
    void checkCompileErrors(unsigned int shader, std::string type);

    unsigned int mID;
};
