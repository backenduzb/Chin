#pragma once

#include "glad/glad.h"
#include <string>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    GLuint id;

    Shader(const char* vertexPath, const char* fragmentPath);
    void setMat4(const std::string& name, const glm::mat4& mat);
    void use();
};