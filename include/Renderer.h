#pragma once
#include "Shader.h"
#include "Camera.h"
#include "GLBLoader.h"
#include <map>
#include <string>

struct GLFWwindow;

class Renderer {
public:
    void clear();
    void render(Shader& shader, Camera& camera, GLBLoader& model, GLFWwindow* window, const std::vector<glm::mat4>& jointMatrices);

private:
    std::map<std::string, GLint> uniformCache;
    GLint getUniformLocation(GLuint shaderId, const std::string& name);
    
    // Cache for some state to avoid redundant OpenGL calls
    GLuint lastVAO = 0;
    GLuint lastTexture = 0;
};
