#include "UIRenderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../utils/FileSystem.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../../external/tinygltf/examples/common/imgui/stb_truetype.h"

UIRenderer::UIRenderer() : bubbleVAO(0), bubbleVBO(0), textVAO(0), textVBO(0), fontTexture(0) {}

UIRenderer::~UIRenderer() {}

void UIRenderer::Init() {
    bubbleShader = std::make_unique<Shader>(Utils::getResourcePath("shaders/ui_bubble.vert").c_str(), Utils::getResourcePath("shaders/ui_bubble.frag").c_str());
    textShader = std::make_unique<Shader>(Utils::getResourcePath("shaders/ui_text.vert").c_str(), Utils::getResourcePath("shaders/ui_text.frag").c_str());

    float quadVertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &bubbleVAO);
    glGenBuffers(1, &bubbleVBO);
    glBindVertexArray(bubbleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bubbleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    LoadFont(Utils::getResourcePath("assets/Roboto-Regular.ttf").c_str());
}

void UIRenderer::LoadFont(const char* fontPath) {
    FILE* file = fopen(fontPath, "rb");
    if (!file) {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
        return;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char* ttfBuffer = new unsigned char[size];
    fread(ttfBuffer, 1, size, file);
    fclose(file);

    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));

    int bitmapWidth = 512;
    int bitmapHeight = 512;
    unsigned char* bitmap = new unsigned char[bitmapWidth * bitmapHeight];
    
    float scale = stbtt_ScaleForPixelHeight(&font, 24.0f);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    stbtt_bakedchar cdata[96];
    stbtt_BakeFontBitmap(ttfBuffer, 0, 24.0f, bitmap, bitmapWidth, bitmapHeight, 32, 96, cdata);

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapWidth, bitmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < 96; i++) {
        characters[i + 32].tx = cdata[i].x0 / (float)bitmapWidth;
        characters[i + 32].ty = cdata[i].y0 / (float)bitmapHeight;
        characters[i + 32].tw = (cdata[i].x1 - cdata[i].x0) / (float)bitmapWidth;
        characters[i + 32].th = (cdata[i].y1 - cdata[i].y0) / (float)bitmapHeight;
        characters[i + 32].size = glm::vec2(cdata[i].x1 - cdata[i].x0, cdata[i].y1 - cdata[i].y0);
        characters[i + 32].bearing = glm::vec2(cdata[i].xoff, -cdata[i].yoff);
        characters[i + 32].advance = (unsigned int)(cdata[i].xadvance);
    }

    delete[] bitmap;
    delete[] ttfBuffer;
}

void UIRenderer::Render(int width, int height, const std::vector<std::shared_ptr<SpeechBubble>>& bubbles, glm::vec2 charScreenPos) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

    for (const auto& bubble : bubbles) {
        RenderBubbleQuad(*bubble, projection);
        
        float textWidth = 0.0f;
        for (char c : bubble->text) {
            unsigned char uc = (unsigned char)c;
            if (uc >= 32 && uc < 128) {
                textWidth += characters[uc].advance;
            }
        }
        
        float textX = bubble->position.x + (bubble->size.x - textWidth) * 0.5f;
        float textY = bubble->position.y + (bubble->size.y) * 0.5f + 8.0f;         
        RenderText(bubble->text, textX, textY, bubble->currentScale, glm::vec3(0.1f), projection, bubble->currentAlpha);
    }

    glEnable(GL_DEPTH_TEST);
}

void UIRenderer::RenderBubbleQuad(const SpeechBubble& bubble, const glm::mat4& projection) {
    bubbleShader->use();
    bubbleShader->setMat4("u_Projection", projection);
    
    glm::vec2 padding(60.0f); 
    glm::vec2 quadSize = bubble.size + padding;
    glm::vec2 position = bubble.position - padding * 0.5f;

    bubbleShader->setVec2("u_Position", position);
    bubbleShader->setVec2("u_QuadSize", quadSize);
    bubbleShader->setVec2("u_BubbleSize", bubble.size);
    bubbleShader->setFloat("u_Scale", bubble.currentScale);
    bubbleShader->setFloat("u_Alpha", bubble.currentAlpha);
    
    int dir = 0;
    if (bubble.tailDirection == BubbleDirection::Left) dir = 0;
    else if (bubble.tailDirection == BubbleDirection::Right) dir = 1;
    else if (bubble.tailDirection == BubbleDirection::Bottom) dir = 2;
    else dir = 3;
    bubbleShader->setInt("u_Direction", dir);

    glBindVertexArray(bubbleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void UIRenderer::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection, float alpha) {
    textShader->use();
    textShader->setMat4("u_Projection", projection);
    textShader->setVec3("u_TextColor", color);
    textShader->setFloat("u_Alpha", alpha);
    textShader->setInt("u_TextTexture", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glBindVertexArray(textVAO);

    
    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        unsigned char chCode = (unsigned char)(*c);
        if (chCode < 32 || chCode >= 128) continue;
        
        Character ch = characters[chCode];

        float xpos = x + ch.bearing.x;
        float ypos = y - ch.bearing.y;

        float w = ch.size.x;
        float h = ch.size.y;

        float vertices[6][4] = {
            { xpos,     ypos + h,   ch.tx, ch.ty + ch.th },            
            { xpos + w, ypos,       ch.tx + ch.tw, ch.ty },
            { xpos,     ypos,       ch.tx, ch.ty },

            { xpos,     ypos + h,   ch.tx, ch.ty + ch.th },
            { xpos + w, ypos + h,   ch.tx + ch.tw, ch.ty + ch.th },
            { xpos + w, ypos,       ch.tx + ch.tw, ch.ty }   
        };

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += ch.advance;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
