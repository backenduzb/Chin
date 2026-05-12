#include "../include/Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

AnimationPlayer::AnimationPlayer() {}

void AnimationPlayer::setAnimation(int index) {
    currentAnimationIndex = index;
    currentTime = 0.0f;
}

void AnimationPlayer::update(float deltaTime, GLBLoader& model) {
    if (currentAnimationIndex < 0 || currentAnimationIndex >= model.animations.size()) {
        globalMatrices.assign(model.nodes.size(), glm::mat4(1.0f));
        for (int root : model.rootNodes) updateNodeTransform(model, root, glm::mat4(1.0f));
        return;
    }

    const Animation& anim = model.animations[currentAnimationIndex];
    currentTime += deltaTime;
    if (currentTime > anim.duration) currentTime = fmod(currentTime, anim.duration);

    for (const auto& channel : anim.channels) {
        Node& node = model.nodes[channel.nodeIndex];
        const AnimationSampler& sampler = anim.samplers[channel.samplerIndex];

        size_t nextIdx = 0;
        while (nextIdx < sampler.inputs.size() && currentTime > sampler.inputs[nextIdx]) nextIdx++;
        if (nextIdx == 0) nextIdx = 1;
        if (nextIdx >= sampler.inputs.size()) nextIdx = sampler.inputs.size() - 1;
        size_t prevIdx = nextIdx - 1;

        float t = (currentTime - sampler.inputs[prevIdx]) / (sampler.inputs[nextIdx] - sampler.inputs[prevIdx]);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        if (channel.path == AnimationChannel::TRANSLATION) {
            glm::vec3 v1 = glm::vec3(sampler.outputs[prevIdx]);
            glm::vec3 v2 = glm::vec3(sampler.outputs[nextIdx]);
            node.translation = glm::mix(v1, v2, t);
        } else if (channel.path == AnimationChannel::ROTATION) {
            glm::quat q1 = glm::make_quat(&sampler.outputs[prevIdx].x);
            glm::quat q2 = glm::make_quat(&sampler.outputs[nextIdx].x);
            node.rotation = glm::slerp(q1, q2, t);
        } else if (channel.path == AnimationChannel::SCALE) {
            glm::vec3 v1 = glm::vec3(sampler.outputs[prevIdx]);
            glm::vec3 v2 = glm::vec3(sampler.outputs[nextIdx]);
            node.scale = glm::mix(v1, v2, t);
        }
    }

    globalMatrices.assign(model.nodes.size(), glm::mat4(1.0f));
    for (int root : model.rootNodes) updateNodeTransform(model, root, glm::mat4(1.0f));
}

void AnimationPlayer::updateNodeTransform(GLBLoader& model, int nodeIndex, const glm::mat4& parentTransform) {
    Node& node = model.nodes[nodeIndex];
    glm::mat4 globalTransform = parentTransform * node.getLocalTransform();
    globalMatrices[nodeIndex] = globalTransform;
    for (int child : node.children) updateNodeTransform(model, child, globalTransform);
}

std::vector<glm::mat4> AnimationPlayer::getJointMatrices(const GLBLoader& model, int skinIndex) {
    if (skinIndex < 0 || skinIndex >= model.skins.size()) return {};
    const Skin& skin = model.skins[skinIndex];
    std::vector<glm::mat4> jointMatrices;
    for (size_t i = 0; i < skin.joints.size(); i++) {
        jointMatrices.push_back(globalMatrices[skin.joints[i]] * skin.inverseBindMatrices[i]);
    }
    return jointMatrices;
}
