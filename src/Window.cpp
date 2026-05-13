#include "../include/Window.h"
#include <iostream>

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 1. Oynani ramkasiz qilish
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // 2. Shaffoflikni yoqish
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    // 3. Har doim hamma oynalarning ustida turishi
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    // 4. MUHIM: Click-through (sichqonchani o'tkazib yuborish)
#ifdef GLFW_MOUSE_PASSTHROUGH
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
#endif

    // 5. Focus-ga qarshi choralar
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

    // X11 Class Name o'rnatish (XWayland ishlaganda kerak bo'ladi)
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "glboverlay");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "glboverlay");

    window = glfwCreateWindow(width, height, "glboverlay", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Oynani to'liq ekran qilish
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
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}
