#include "Window.h"
#include <iostream>

#ifndef GLFW_WAYLAND_APP_ID
#define GLFW_WAYLAND_APP_ID 0x00024001
#endif

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
    
#ifdef GLFW_MOUSE_PASSTHROUGH
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
#endif

    glfwWindowHintString(GLFW_X11_CLASS_NAME, "chin");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "chin");
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "chin");

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

#ifdef GLFW_MOUSE_PASSTHROUGH
    glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
#endif

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (mode) {
        glfwSetWindowSize(window, mode->width, mode->height);
        glfwSetWindowPos(window, 0, 0);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return window ? glfwWindowShouldClose(window) : true;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
    }
}
