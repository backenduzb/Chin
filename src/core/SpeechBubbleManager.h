#pragma once
#include <vector>
#include <memory>
#include "SpeechBubble.h"

class SpeechBubbleManager {
public:
    void ShowMessage(const std::string& message);
    void Update(float dt, glm::vec2 characterScreenPos, glm::vec2 viewportSize);
    const std::vector<std::shared_ptr<SpeechBubble>>& GetActiveBubbles() const { return bubbles; }
private:
    std::vector<std::shared_ptr<SpeechBubble>> bubbles;
    void CalculateLayout(SpeechBubble& bubble, glm::vec2 charPos, glm::vec2 viewport);
};
