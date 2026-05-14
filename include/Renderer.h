#pragma once
#include "Shader.h"
#include "Camera.h"
#include "GLBLoader.h"
#include <map>
#include <string>
#include <glm/glm.hpp>

struct GLFWwindow;

class Renderer {
public:
    void clear();
    void render(Shader& shader, Camera& camera, GLBLoader& model, GLFWwindow* window, const std::vector<glm::mat4>& jointMatrices, const glm::mat4& modelMat = glm::mat4(1.0f));

private:
    std::map<std::string, GLint> uniformCache;
    GLint getUniformLocation(GLuint shaderId, const std::string& name);
    
    GLuint lastVAO = 0;
    GLuint lastTexture = 0;
};
