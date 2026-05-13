#include "../include/Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

void Renderer::clear() {
    // Fonni to'liq shaffof qilish (Alpha = 0.0)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLint Renderer::getUniformLocation(GLuint shaderId, const std::string& name) {
    std::string key = std::to_string(shaderId) + name;
    if (uniformCache.find(key) != uniformCache.end()) return uniformCache[key];
    GLint location = glGetUniformLocation(shaderId, name.c_str());
    uniformCache[key] = location;
    return location;
}

void Renderer::render(Shader& shader, Camera& camera, GLBLoader& model, GLFWwindow* window, const std::vector<glm::mat4>& jointMatrices) {
    shader.use();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLint modelLoc = getUniformLocation(shader.id, "model");
    GLint viewLoc = getUniformLocation(shader.id, "view");
    GLint projLoc = getUniformLocation(shader.id, "projection");
    GLint skinnedLoc = getUniformLocation(shader.id, "isSkinned");
    GLint jointsLoc = getUniformLocation(shader.id, "jointMatrices");

    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::mat4 viewMat = camera.getView();
    glm::mat4 projMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    if (!jointMatrices.empty()) {
        glUniform1i(skinnedLoc, 1);
        glUniformMatrix4fv(jointsLoc, (GLsizei)jointMatrices.size(), GL_FALSE, glm::value_ptr(jointMatrices[0]));
    } else {
        glUniform1i(skinnedLoc, 0);
    }

    GLint diffuseLoc = getUniformLocation(shader.id, "diffuseTexture");
    GLint hasTexLoc = getUniformLocation(shader.id, "hasTexture");
    
    for (auto& p : model.primitives) {
        if (p.materialIndex >= 0 && (size_t)p.materialIndex < model.textures.size()) {
            GLuint tex = model.textures[p.materialIndex];
            if (tex != lastTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex);
                lastTexture = tex;
                glUniform1i(diffuseLoc, 0);
            }
            glUniform1i(hasTexLoc, 1);
        } else {
            glUniform1i(hasTexLoc, 0);
        }

        if (p.vao != lastVAO) {
            glBindVertexArray(p.vao);
            lastVAO = p.vao;
        }
        glDrawElements(GL_TRIANGLES, p.indexCount, p.indexType, 0);
    }
}
