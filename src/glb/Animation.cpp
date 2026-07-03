#include "./Animation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cctype>

AnimationPlayer::AnimationPlayer() {}

void AnimationPlayer::setAnimation(int index) {
    currentAnimationIndex = index;
    currentTime = 0.0f;
}

void AnimationPlayer::setAnimation(const std::string& name, const GLBLoader& model) {
    auto toLower = [](const std::string& s) {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return lower;
    };
    
    std::string lowerTarget = toLower(name);
    for (size_t i = 0; i < model.animations.size(); i++) {
        if (toLower(model.animations[i].name) == lowerTarget) {
            currentAnimationIndex = (int)i;
            currentTime = 0.0f;
            return;
        }
    }
    std::cerr << "[Warning] Animation with name '" << name << "' not found!" << std::endl;
}

void AnimationPlayer::update(float deltaTime, GLBLoader& model) {
    if (currentAnimationIndex < 0 || static_cast<size_t>(currentAnimationIndex) >= model.animations.size()) {
        globalMatrices.assign(model.nodes.size(), glm::mat4(1.0f));
        for (int root : model.rootNodes) updateNodeTransform(model, root, glm::mat4(1.0f));
        return;
    }

    const Animation& anim = model.animations[currentAnimationIndex];
    currentTime += deltaTime;
    if (currentTime > anim.duration) currentTime = std::fmod(currentTime, anim.duration);

    for (const auto& channel : anim.channels) {
        if (channel.nodeIndex < 0 || static_cast<size_t>(channel.nodeIndex) >= model.nodes.size()) continue;
        Node& node = model.nodes[channel.nodeIndex];
        node.useMatrix = false;
        const AnimationSampler& sampler = anim.samplers[channel.samplerIndex];

        if (sampler.inputs.empty() || (sampler.outputs.empty() && sampler.outputWeights.empty())) continue;

        bool isCubicspline = (sampler.interpolation == AnimationSampler::CUBICSPLINE);
        if (isCubicspline && sampler.outputs.size() < 3 * sampler.inputs.size() && sampler.outputWeights.size() < 3 * sampler.inputs.size()) {
            isCubicspline = false; 
        }

        size_t numTargets = 0;
        if (channel.path == AnimationChannel::WEIGHTS && !sampler.inputs.empty()) {
            numTargets = sampler.outputWeights.size() / sampler.inputs.size() / (isCubicspline ? 3 : 1);
        }
        
        auto getWeightsValue = [&](size_t idx, int subType = 1) -> std::vector<float> {
            std::vector<float> res(numTargets, 0.0f);
            size_t base = isCubicspline ? (3 * idx + subType) * numTargets : idx * numTargets;
            for(size_t i=0; i<numTargets; i++) {
                if (base + i < sampler.outputWeights.size()) res[i] = sampler.outputWeights[base + i];
            }
            return res;
        };

        auto getVec3Value = [&](size_t idx) -> glm::vec3 {
            if (isCubicspline) {
                size_t base = 3 * idx + 1;
                if (base < sampler.outputs.size()) return glm::vec3(sampler.outputs[base]);
            }
            if (idx < sampler.outputs.size()) return glm::vec3(sampler.outputs[idx]);
            return glm::vec3(0.0f);
        };

        auto getQuatValue = [&](size_t idx) -> glm::quat {
            if (isCubicspline) {
                size_t base = 3 * idx + 1;
                if (base < sampler.outputs.size()) return glm::make_quat(&sampler.outputs[base].x);
            }
            if (idx < sampler.outputs.size()) return glm::make_quat(&sampler.outputs[idx].x);
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        };

        if (sampler.inputs.size() == 1) {
            if (channel.path == AnimationChannel::TRANSLATION) {
                node.translation = getVec3Value(0);
            } else if (channel.path == AnimationChannel::ROTATION) {
                node.rotation = getQuatValue(0);
            } else if (channel.path == AnimationChannel::SCALE) {
                node.scale = getVec3Value(0);
            } else if (channel.path == AnimationChannel::WEIGHTS) {
                node.weights = getWeightsValue(0);
            }
            continue;
        }

        size_t nextIdx = 0;
        while (nextIdx < sampler.inputs.size() && currentTime > sampler.inputs[nextIdx]) nextIdx++;
        if (nextIdx == 0) nextIdx = 1;
        if (nextIdx >= sampler.inputs.size()) nextIdx = sampler.inputs.size() - 1;
        size_t prevIdx = nextIdx - 1;

        float t = (currentTime - sampler.inputs[prevIdx]) / (sampler.inputs[nextIdx] - sampler.inputs[prevIdx]);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        float dt = sampler.inputs[nextIdx] - sampler.inputs[prevIdx];

        if (isCubicspline) {
            if (channel.path == AnimationChannel::TRANSLATION) {
                glm::vec3 p0 = getVec3Value(prevIdx);
                glm::vec3 p1 = getVec3Value(nextIdx);
                glm::vec3 m0 = dt * ((3 * prevIdx + 2 < sampler.outputs.size()) ? glm::vec3(sampler.outputs[3 * prevIdx + 2]) : glm::vec3(0.0f));
                glm::vec3 m1 = dt * ((3 * nextIdx < sampler.outputs.size()) ? glm::vec3(sampler.outputs[3 * nextIdx]) : glm::vec3(0.0f));
                node.translation = (2.0f * t * t * t - 3.0f * t * t + 1.0f) * p0 +
                                   (t * t * t - 2.0f * t * t + t) * m0 +
                                   (-2.0f * t * t * t + 3.0f * t * t) * p1 +
                                   (t * t * t - t * t) * m1;
            } else if (channel.path == AnimationChannel::ROTATION) {
                glm::quat p0 = getQuatValue(prevIdx);
                glm::quat p1 = getQuatValue(nextIdx);
                glm::quat m0 = dt * ((3 * prevIdx + 2 < sampler.outputs.size()) ? glm::make_quat(&sampler.outputs[3 * prevIdx + 2].x) : glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
                glm::quat m1 = dt * ((3 * nextIdx < sampler.outputs.size()) ? glm::make_quat(&sampler.outputs[3 * nextIdx].x) : glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
                glm::quat q = (2.0f * t * t * t - 3.0f * t * t + 1.0f) * p0 +
                              (t * t * t - 2.0f * t * t + t) * m0 +
                              (-2.0f * t * t * t + 3.0f * t * t) * p1 +
                              (t * t * t - t * t) * m1;
                node.rotation = glm::normalize(q);
            } else if (channel.path == AnimationChannel::SCALE) {
                glm::vec3 p0 = getVec3Value(prevIdx);
                glm::vec3 p1 = getVec3Value(nextIdx);
                glm::vec3 m0 = dt * ((3 * prevIdx + 2 < sampler.outputs.size()) ? glm::vec3(sampler.outputs[3 * prevIdx + 2]) : glm::vec3(0.0f));
                glm::vec3 m1 = dt * ((3 * nextIdx < sampler.outputs.size()) ? glm::vec3(sampler.outputs[3 * nextIdx]) : glm::vec3(0.0f));
                node.scale = (2.0f * t * t * t - 3.0f * t * t + 1.0f) * p0 +
                             (t * t * t - 2.0f * t * t + t) * m0 +
                             (-2.0f * t * t * t + 3.0f * t * t) * p1 +
                             (t * t * t - t * t) * m1;
            } else if (channel.path == AnimationChannel::WEIGHTS) {
                std::vector<float> p0 = getWeightsValue(prevIdx, 1);
                std::vector<float> p1 = getWeightsValue(nextIdx, 1);
                std::vector<float> m0 = getWeightsValue(prevIdx, 2);
                std::vector<float> m1 = getWeightsValue(nextIdx, 0);
                node.weights.resize(numTargets);
                for (size_t i=0; i<numTargets; i++) {
                    node.weights[i] = (2.0f * t * t * t - 3.0f * t * t + 1.0f) * p0[i] +
                                      (t * t * t - 2.0f * t * t + t) * dt * m0[i] +
                                      (-2.0f * t * t * t + 3.0f * t * t) * p1[i] +
                                      (t * t * t - t * t) * dt * m1[i];
                }
            }
        } else {
            if (sampler.interpolation == AnimationSampler::STEP) {
                if (channel.path == AnimationChannel::TRANSLATION) {
                    node.translation = getVec3Value(prevIdx);
                } else if (channel.path == AnimationChannel::ROTATION) {
                    node.rotation = getQuatValue(prevIdx);
                } else if (channel.path == AnimationChannel::SCALE) {
                    node.scale = getVec3Value(prevIdx);
                } else if (channel.path == AnimationChannel::WEIGHTS) {
                    node.weights = getWeightsValue(prevIdx);
                }
            } else {
                if (channel.path == AnimationChannel::TRANSLATION) {
                    node.translation = glm::mix(getVec3Value(prevIdx), getVec3Value(nextIdx), t);
                } else if (channel.path == AnimationChannel::ROTATION) {
                    node.rotation = glm::slerp(getQuatValue(prevIdx), getQuatValue(nextIdx), t);
                } else if (channel.path == AnimationChannel::SCALE) {
                    node.scale = glm::mix(getVec3Value(prevIdx), getVec3Value(nextIdx), t);
                } else if (channel.path == AnimationChannel::WEIGHTS) {
                    std::vector<float> w0 = getWeightsValue(prevIdx);
                    std::vector<float> w1 = getWeightsValue(nextIdx);
                    node.weights.resize(numTargets);
                    for (size_t i=0; i<numTargets; i++) {
                        node.weights[i] = w0[i] * (1.0f - t) + w1[i] * t;
                    }
                }
            }
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
    if (skinIndex < 0 || static_cast<size_t>(skinIndex) >= model.skins.size()) return {};
    const Skin& skin = model.skins[skinIndex];
    std::vector<glm::mat4> jointMatrices;
    for (size_t i = 0; i < skin.joints.size(); i++) {
        jointMatrices.push_back(globalMatrices[skin.joints[i]] * skin.inverseBindMatrices[i]);
    }
    return jointMatrices;
}
