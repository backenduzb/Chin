#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../core/SpeechBubbleManager.h"
#include "../rendering/Shader.h"

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();
    void Init();
    void Render(int width, int height, const std::vector<std::shared_ptr<SpeechBubble>>& bubbles, glm::vec2 charScreenPos);

private:
    GLuint bubbleVAO, bubbleVBO;
    GLuint textVAO, textVBO;
    std::unique_ptr<Shader> bubbleShader;
    std::unique_ptr<Shader> textShader;
    GLuint fontTexture;
    
    struct Character {
        glm::vec2 size;
        glm::vec2 bearing;
        unsigned int advance;
        float tx, ty, tw, th;
    };
    Character characters[128];
    
    void LoadFont(const char* fontPath);
    void RenderBubbleQuad(const SpeechBubble& bubble, const glm::mat4& projection);
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection, float alpha);
};
