#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    GLuint id = 0;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void setMat4(const std::string& name, const glm::mat4& mat);
    void setVec2(const std::string& name, const glm::vec2& vec);
    void setVec3(const std::string& name, const glm::vec3& vec);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void use();
};
