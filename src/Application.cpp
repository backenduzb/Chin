#include "../include/Application.h"
#include <chrono>
#include <thread>
#include <iostream>


Application::Application() 
    : window(800, 600, "GLB Viewer Animated"),
      shader("/home/vic/Projects/UPGSkills/CppApp/shaders/base.vert", 
             "/home/vic/Projects/UPGSkills/CppApp/shaders/base.frag") 
{
    if (model.load("/home/vic/Projects/UPGSkills/CppApp/assets/bonel.glb")) {
    if (!model.animations.empty()) {
        std::cout << "Starting animation: " << model.animations[0].name << std::endl;
        animPlayer.setAnimation(0);
    }
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

        animPlayer.update(deltaTime.count(), model);
        
        std::vector<glm::mat4> joints;
        if (!model.skins.empty()) {
            joints = animPlayer.getJointMatrices(model, 0);
        }

        renderer.clear();
        renderer.render(shader, camera, model, window.getNativeWindow(), joints);

        window.swapBuffers();

        auto frameEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = frameEndTime - frameStartTime;

        if (frameDuration.count() < targetFrameTime) {
            double sleepTime = targetFrameTime - frameDuration.count();
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
}

void Application::processInput() {
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);
}
