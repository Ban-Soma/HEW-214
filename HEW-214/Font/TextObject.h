#pragma once

#include "TextMesh.h"
#include "TextLayout.h"
#include <string>

class FontSystem;

class TextObject
{
public:

    float Clamp(float value, float low, float high);

    explicit TextObject(FontSystem& fontSystem);

    bool SetText(const std::string& utf8Text);

    bool SetPosition(float x, float y);

    void SetColor(float r,float g,float b,float a);

    void Draw();

    const std::string& GetText() const{
        return m_text; }

    float GetX() const{
        return m_x;}

    float GetY() const{
        return m_y;}


    void SetScale(float scale);

    void SetMaxWidth(float maxWidth);

    void SetLineSpacing(float lineSpacing);

    void SetAlignment(TextAlignment alignment);

    float GetMaxWidth() const
    {
        return m_maxWidth;
    }

    float GetLineSpacing() const
    {
        return m_lineSpacing;
    }

    TextAlignment GetAlignment() const
    {
        return m_alignment;
    }

    float GetTextWidth() const
    {
        return m_textWidth;
    }

    float GetTextHeight() const
    {
        return m_textHeight;
    }
private:
    bool RebuildMesh();

private:
    FontSystem* m_fontSystem = nullptr;

    std::string m_text;
    TextMesh m_mesh;
    bool m_dirty = true;
    float m_x = 0.0f;
    float m_y = 0.0f;

    float m_r = 1.0f;
    float m_g = 1.0f;
    float m_b = 1.0f;
    float m_a = 1.0f;

    float m_scale = 1.0f;
    float m_maxWidth = 0.0f;
    float m_lineSpacing = 0.0f;

    float m_textWidth = 0.0f;
    float m_textHeight = 0.0f;

    TextAlignment m_alignment =TextAlignment::Left;
};