#pragma once
#include "GLBLoader.h"
#include <vector>
#include <glm/glm.hpp>

class AnimationPlayer {
public:
    AnimationPlayer();
    
    void setAnimation(int index);
    void setAnimation(const std::string& name, const GLBLoader& model);
    void update(float deltaTime, GLBLoader& model);
    
    std::vector<glm::mat4> getJointMatrices(const GLBLoader& model, int skinIndex);

private:
    int currentAnimationIndex = -1;
    float currentTime = 0.0f;

    void updateNodeTransform(GLBLoader& model, int nodeIndex, const glm::mat4& parentTransform);
    std::vector<glm::mat4> globalMatrices;
};
