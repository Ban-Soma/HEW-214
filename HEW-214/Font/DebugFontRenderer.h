#pragma once

#include "IFontRenderer.h"

#include <cstddef>

class DebugFontRenderer final
    : public IFontRenderer
{
public:
    void DrawTextMesh(
        const TextMesh& mesh,
        const TextureAtlas& atlas
    ) override;

    size_t GetLastVertexCount() const
    {
        return m_lastVertexCount;
    }

    size_t GetLastIndexCount() const
    {
        return m_lastIndexCount;
    }

    int GetLastAtlasWidth() const
    {
        return m_lastAtlasWidth;
    }

    int GetLastAtlasHeight() const
    {
        return m_lastAtlasHeight;
    }

private:
    size_t m_lastVertexCount = 0;
    size_t m_lastIndexCount = 0;

    int m_lastAtlasWidth = 0;
    int m_lastAtlasHeight = 0;
};
