#pragma once
#include <string>
#include <glm/glm.hpp>

enum class BubbleState { Spawning, Visible, Fading, Dead };
enum class BubbleDirection { Right, Left, Top, Bottom };

struct SpeechBubble {
    std::string text;
    BubbleState state = BubbleState::Spawning;
    
    glm::vec2 position{0.0f};
    glm::vec2 targetPosition{0.0f};
    glm::vec2 size{0.0f};
    BubbleDirection tailDirection = BubbleDirection::Left;
    
    float lifetimeTimer = 4.0f;
    float animProgress = 0.0f;
    const float ANIM_DURATION = 0.35f;
    
    float currentScale = 0.9f;
    float currentAlpha = 0.0f;
    
    SpeechBubble(const std::string& msg) : text(msg) {}
    void Update(float dt);
};
