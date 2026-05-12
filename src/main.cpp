#include "../include/glad/glad.h"

#include <GLFW/glfw3.h>

#include "../include/Shader.h"
#include "../include/Camera.h"
#include "../include/GLBLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window =
        glfwCreateWindow(
            800,
            600,
            "GLB Viewer",
            nullptr,
            nullptr
        );
    glfwSetWindowPos(window, 10, 10);

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glEnable(GL_DEPTH_TEST);

    Shader shader(
        "/home/vic/Projects/UPGSkills/CppApp/shaders/base.vert",
        "/home/vic/Projects/UPGSkills/CppApp/shaders/base.frag"
    );

    Camera camera;

    GLBLoader model;

    model.load("/home/vic/Projects/UPGSkills/CppApp/assets/model.glb");

    GLuint modelLoc =
        glGetUniformLocation(shader.id, "model");

    GLuint viewLoc =
        glGetUniformLocation(shader.id, "view");

    GLuint projLoc =
        glGetUniformLocation(shader.id, "projection");

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        float time = glfwGetTime();

        glm::mat4 modelMat = glm::mat4(1.0f);

        modelMat =
            glm::rotate(
                modelMat,
                time,
                glm::vec3(0.2f, 1.0f, 0.0f)
            );

        glm::mat4 viewMat = camera.getView();

        glm::mat4 projMat =
            glm::perspective(
                glm::radians(45.0f),
                1000.0f / 1000.0f,
                0.1f,
                100.0f
            );

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(modelMat)
        );

        glUniformMatrix4fv(
            viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(viewMat)
        );

        glUniformMatrix4fv(
            projLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projMat)
        );

        model.draw();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}