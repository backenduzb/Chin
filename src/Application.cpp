#include "../include/Application.h"

Application::Application() 
    : window(800, 600, "GLB Viewer"),
      shader("/home/vic/Projects/UPGSkills/CppApp/shaders/base.vert", 
             "/home/vic/Projects/UPGSkills/CppApp/shaders/base.frag") 
{
    model.load("/home/vic/Projects/UPGSkills/CppApp/assets/model.glb");
}

Application::~Application() {
}

void Application::run() {
    while (!window.shouldClose()) {
        window.pollEvents();
        processInput();

        renderer.clear();
        renderer.render(shader, camera, model, window.getNativeWindow());

        window.swapBuffers();
    }
}

void Application::processInput() {
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);
}
