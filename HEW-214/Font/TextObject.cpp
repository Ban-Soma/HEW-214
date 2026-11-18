#include <algorithm>
#include "TextObject.h"
#include "FontSystem.h"

TextObject::TextObject(FontSystem& fontSystem):m_fontSystem(&fontSystem)
{
}

bool TextObject::SetText(const std::string& utf8Text)
{
    if (m_text == utf8Text) return true;
    m_text = utf8Text;
    m_dirty = true;
    return true;
}

bool TextObject::SetPosition(float x,float y)
{
    m_x = x;
    m_y = y;



    return true;
}

void TextObject::SetColor(float r,float g,float b,float a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}
float TextObject::Clamp(float value, float low, float high)
{
    return (std::min)(high, (std::max)(low, value));
}

void TextObject::SetScale(float scale)
{
    scale = Clamp(scale, 0.1f, 10.0f);
    if (m_scale == scale) return;
    m_scale = scale;
    m_dirty = true;
}

void TextObject::SetMaxWidth(float maxWidth)
{
    maxWidth = (std::max)(0.0f, maxWidth);

    if (m_maxWidth == maxWidth)
        return;

    m_maxWidth = maxWidth;
    m_dirty = true;
}
void TextObject::SetLineSpacing(float lineSpacing)
{
    if (m_lineSpacing == lineSpacing)
        return;

    m_lineSpacing = lineSpacing;
    m_dirty = true;
}

void TextObject::SetAlignment(
    TextAlignment alignment)
{
    if (m_alignment == alignment)
        return;

    m_alignment = alignment;
    m_dirty = true;
}
bool TextObject::RebuildMesh()
{
    if (!m_fontSystem)
        return false;

    if (!m_fontSystem->IsInitialized())
        return false;

    if (m_text.empty())
    {
        m_mesh.Clear();

        m_textWidth = 0.0f;
        m_textHeight = 0.0f;
        return true;
    }
    TextLayout layout;

    layout.maxWidth = m_maxWidth / m_scale;
    layout.lineSpacing = m_lineSpacing / m_scale;
    layout.alignment = m_alignment;
   
    m_mesh = m_fontSystem->BuildTextMesh(m_text,layout,m_textWidth,m_textHeight);
    for (TextVertex& vertex : m_mesh.vertices)
    {
        vertex.x *= m_scale;
        vertex.y *= m_scale;
    }
    m_textWidth *= m_scale;
    m_textHeight *= m_scale;

    return m_mesh.IsValid();
}

void TextObject::Draw()
{
    if (!m_fontSystem)return;
    if (m_dirty)
    {
        if (!RebuildMesh())
            return;
        m_dirty = false;
    }
    if (!m_fontSystem) return;

    m_fontSystem->DrawMesh( m_mesh,m_x,m_y,m_r,m_g, m_b,m_a );
}