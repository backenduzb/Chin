#pragma once
#include "Window.h"
#include "../rendering/Renderer.h"
#include "../rendering/Shader.h"
#include "../rendering/Camera.h"
#include "../glb/GLBLoader.h"
#include "../glb/Animation.h"
#include "SpeechBubbleManager.h"
#include "../rendering/UIRenderer.h"

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
    AnimationPlayer animPlayer;
    SpeechBubbleManager bubbleManager;
    UIRenderer uiRenderer;
    void processInput();
};
