#include "SpeechBubbleManager.h"
#include <algorithm>
#include <iostream>

constexpr float ANIM_DURATION = 0.3f; 
constexpr float FADE_DURATION = 0.25f;

static float easeOutBack(float x) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    const float xM1 = x - 1.0f;
    const float xM1Sq = xM1 * xM1;
    return 1.0f + c3 * (xM1Sq * xM1) + c1 * xM1Sq;
}

void SpeechBubble::Update(float dt) {
    switch (state) {
        case BubbleState::Spawning: {
            animProgress += (dt / ANIM_DURATION);
            if (animProgress >= 1.0f) {
                animProgress = 1.0f;
                state = BubbleState::Visible;
            }
            
            float ease = easeOutBack(animProgress);
            currentAlpha = animProgress;
            currentScale = 0.8f + (0.2f * ease);
            position.x = targetPosition.x;
            position.y = targetPosition.y + (15.0f * (1.0f - ease));
            break;
        }
            
        case BubbleState::Visible:
            lifetimeTimer -= dt;
            if (lifetimeTimer <= 0.0f) {
                state = BubbleState::Fading;
                animProgress = 1.0f;
            }
            position = targetPosition;
            break;
            
        case BubbleState::Fading:
            animProgress -= (dt / FADE_DURATION);
            if (animProgress <= 0.0f) {
                animProgress = 0.0f;
                state = BubbleState::Dead;
            }
            currentAlpha = animProgress;
            currentScale = 0.8f + (0.2f * animProgress);
            position = targetPosition;
            break;
            
        case BubbleState::Dead:
            break;
    }
}

void SpeechBubbleManager::ShowMessage(const std::string& message) {
    bubbles.clear();
    bubbles.push_back(std::make_shared<SpeechBubble>(message));
}

void SpeechBubbleManager::Update(float dt, glm::vec2 charPos, glm::vec2 viewport) {
    for (auto it = bubbles.begin(); it != bubbles.end();) {
        auto& b = **it;

        float textLength = static_cast<float>(b.text.length()); 

        if (textLength == 0.0f) {
            textLength = 10.0f; 
        }

        float calculatedWidth = (textLength * 9.5f) + 30.0f; 
        float calculatedHeight = 45.0f; 

        b.size = glm::vec2(calculatedWidth, calculatedHeight);

        CalculateLayout(b, charPos, viewport);
        
        b.Update(dt);

        if (b.state == BubbleState::Dead) {
            it = bubbles.erase(it);
        } else {
            ++it;
        }
    }
}

void SpeechBubbleManager::CalculateLayout(SpeechBubble& b, glm::vec2 charPos, glm::vec2 viewport) {
    static constexpr float verticalOffset = 380.0f;
    static constexpr float horizontalOffset = 60.0f;
    static constexpr float padding = 16.0f;

    glm::vec2 target;

    target.y = charPos.y - b.size.y - verticalOffset;

    target.x = charPos.x - b.size.x - horizontalOffset;

    b.tailDirection = BubbleDirection::Top;

    target.x = std::clamp(target.x, padding, viewport.x - b.size.x - padding);
    target.y = std::clamp(target.y, padding, viewport.y - b.size.y - padding);

    b.targetPosition = target;

    if (b.state != BubbleState::Spawning) {
        b.position = target;
    }
}