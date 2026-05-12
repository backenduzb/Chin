#pragma once

#include "glad/glad.h"

class GLBLoader {
public:

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int indexCount;
    GLenum indexType;

    bool load(const char* path);

    void draw();
};