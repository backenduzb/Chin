#pragma once

#include "glad/glad.h"

class Shader {
public:
    GLuint id;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use();
};