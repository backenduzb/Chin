#include "Shader.h"
#include "../utils/FileSystem.h"
#include <iostream>

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[Shader Error] Compilation failed:\n" << infoLog << std::endl;
    }
    return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vsCode = Utils::readFile(vertexPath);
    std::string fsCode = Utils::readFile(fragmentPath);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vsCode.c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsCode.c_str());

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "[Shader Error] Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec2(const std::string& name, const glm::vec2& vec) {
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &vec[0]);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
