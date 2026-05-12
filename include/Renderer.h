#pragma once
#include "Shader.h"
#include "Camera.h"
#include "GLBLoader.h"

struct GLFWwindow;

class Renderer {
public:
    void clear();
    void render(Shader& shader, Camera& camera, GLBLoader& model, GLFWwindow* window);
};
