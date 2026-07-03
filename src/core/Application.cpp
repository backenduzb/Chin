#include "Application.h"
#include "../utils/FileSystem.h"
#include <chrono>
#include <string>
#include <thread>
#include <iostream>

Application::Application()
    : window(300, 300, "chin"),
      shader(Utils::getResourcePath("shaders/base.vert"),
             Utils::getResourcePath("shaders/base.frag"))
{
    std::string modelPath = Utils::getResourcePath("assets/Untitled.glb");
    if (model.load(modelPath.c_str())) {
        if (!model.animations.empty()) {
            animPlayer.setAnimation("faceangry", model);
        }
        animPlayer.update(0.0f, model);
        model.updateMorphTargets();
    } else {
        std::cerr << "[Error] Failed to load model: " << modelPath << std::endl;
    }
    
    uiRenderer.Init();
    bubbleManager.ShowMessage("Ajoyib bublebox ishladi qoyil kutilgan natija endi filelarni seaparate qilish kerak");
    bubbleManager.ShowMessage("Bu oxirgi message");
}

Application::~Application() {
}

void Application::run() {
    const double targetFPS = 60.0;
    const double targetFrameTime = 1.0 / targetFPS;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!window.shouldClose()) {
        auto frameStartTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = frameStartTime - lastTime;
        lastTime = frameStartTime;

        window.pollEvents();
        processInput();

        if (deltaTime.count() > 0.001f) {
            animPlayer.update(deltaTime.count(), model);
            model.updateMorphTargets();
        }

        std::vector<glm::mat4> joints;
        if (!model.skins.empty()) {
            joints = animPlayer.getJointMatrices(model, 0);
        }
        renderer.clear();

        glm::mat4 modelMat(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(3.0f, -1.0f,    0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(1.0f), glm::vec3(3.0f, 2.0f, 3.0f));

        renderer.render(shader, camera, model, window.getNativeWindow(), joints, modelMat);

        int width, height;
        glfwGetWindowSize(window.getNativeWindow(), &width, &height);
        
        glm::mat4 view = camera.getView();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
        glm::vec3 charWorldPos = glm::vec3(modelMat[3]); // roughly position of the character
        
        glm::vec4 clipSpacePos = proj * view * glm::vec4(charWorldPos, 1.0f);
        glm::vec2 ndcPos = glm::vec2(clipSpacePos.x, clipSpacePos.y) / clipSpacePos.w;
        glm::vec2 screenPos = ((ndcPos + 1.0f) / 2.0f) * glm::vec2(width, height);
        screenPos.y = height - screenPos.y; 
        
        bubbleManager.Update(deltaTime.count(), screenPos, glm::vec2(width, height));
        uiRenderer.Render(width, height, bubbleManager.GetActiveBubbles(), screenPos);

        window.swapBuffers();

        auto frameEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = frameEndTime - frameStartTime;

        if (frameDuration.count() < targetFrameTime) {
            double sleepTime = targetFrameTime - frameDuration.count();
            if (sleepTime > 0.002) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime - 0.001));
            }
            while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - frameStartTime).count() < targetFrameTime);
        }
    }
}

void Application::processInput() {
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);
        
    static bool tPressed = false;
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_T) == GLFW_PRESS) {
        if (!tPressed) {
            bubbleManager.ShowMessage("Salom 👋");
            tPressed = true;
        }
    } else {
        tPressed = false;
    }
}
