#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Node {
    int index;
    std::string name;
    int parent = -1;
    std::vector<int> children;
    
    glm::vec3 translation = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 matrix = glm::mat4(1.0f);
    bool useMatrix = false;

    int meshIndex = -1;
    int skinIndex = -1;

    glm::mat4 getLocalTransform() {
        if (useMatrix) return matrix;
        return glm::translate(glm::mat4(1.0f), translation) * 
               glm::mat4_cast(rotation) * 
               glm::scale(glm::mat4(1.0f), scale);
    }
};

struct Skin {
    std::string name;
    std::vector<int> joints;
    std::vector<glm::mat4> inverseBindMatrices;
};

struct AnimationSampler {
    enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
    InterpolationType interpolation;
    std::vector<float> inputs;
    std::vector<glm::vec4> outputs;
};

struct AnimationChannel {
    enum PathType { TRANSLATION, ROTATION, SCALE };
    PathType path;
    int nodeIndex;
    int samplerIndex;
};

struct Animation {
    std::string name;
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
    float duration = 0.0f;
};

struct Primitive {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int indexCount;
    GLenum indexType;
    int materialIndex;
};

class GLBLoader {
public:
    std::vector<Primitive> primitives;
    std::vector<GLuint> textures;
    std::vector<Node> nodes;
    std::vector<Skin> skins;
    std::vector<Animation> animations;
    std::vector<int> rootNodes;

    bool load(const char* path);
    void draw(GLuint shaderId);

private:
    std::map<std::string, GLint> uniformCache;
    GLint getUniformLocation(GLuint shaderId, const std::string& name);
};
