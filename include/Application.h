#pragma once
#include "Window.h"
#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "GLBLoader.h"

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    Window window;
    Renderer renderer;
    Shader shader;
    Camera camera;
    GLBLoader model;

    void processInput();
};
