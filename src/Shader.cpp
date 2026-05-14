#include "../include/Shader.h"
#include "../include/Reader.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLuint compile(GLenum type, const char* src) {

    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char info[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info);
        std::cout << info << '\n';
    }

    return shader;
}

Shader::Shader(
    const char* vertexPath,
    const char* fragmentPath
) {

    std::string vsCode = readFile(vertexPath);
    std::string fsCode = readFile(fragmentPath);

    GLuint vs = compile(GL_VERTEX_SHADER, vsCode.c_str());
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsCode.c_str());

    id = glCreateProgram();

    glAttachShader(id, vs);
    glAttachShader(id, fs);

    glLinkProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(
        glGetUniformLocation(id, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(mat)
    );
}