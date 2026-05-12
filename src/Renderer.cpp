#include "../include/Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Shader& shader, Camera& camera, GLBLoader& model, GLFWwindow* window) {
    shader.use();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    GLuint modelLoc = glGetUniformLocation(shader.id, "model");
    GLuint viewLoc = glGetUniformLocation(shader.id, "view");
    GLuint projLoc = glGetUniformLocation(shader.id, "projection");

    float time = glfwGetTime();
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::rotate(modelMat, time, glm::vec3(0.2f, 1.0f, 0.0f));

    glm::mat4 viewMat = camera.getView();
    glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    model.draw();
}
