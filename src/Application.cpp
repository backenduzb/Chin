#include "../include/Application.h"
#include <chrono>
#include <thread>

Application::Application() 
    : window(300, 300, "chin"),
      shader("/home/vic/Projects/UPGSkills/CppApp/shaders/base.vert", 
             "/home/vic/Projects/UPGSkills/CppApp/shaders/base.frag") 
{
	if (model.load("/home/vic/Projects/UPGSkills/CppApp/assets/bonel.glb")) {
        if (!model.animations.empty()) {
            animPlayer.setAnimation(2);
        }
        animPlayer.update(0.0f, model);
    }
}

Application::~Application() {
}

void Application::run() {
    const double targetFPS = 45.0;
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
        }
        
        std::vector<glm::mat4> joints;
        if (!model.skins.empty()) {
            joints = animPlayer.getJointMatrices(model, 0);
        }
        renderer.clear();
        
        glm::mat4 modelMat(1.0f);
        
        modelMat = glm::translate(modelMat, glm::vec3(3.0f, -1.5f, 0.0f));
        
        modelMat = glm::rotate(modelMat, glm::radians(1.0f), glm::vec3(3.0f, 2.0f, 3.0f));
        
        renderer.render(shader, camera, model, window.getNativeWindow(), joints, modelMat);

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
}
