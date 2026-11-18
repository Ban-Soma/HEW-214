#pragma once

#include "GlyphBitmap.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

struct AtlasRegion
{
    int x = 0;
    int y = 0;

    int width = 0;
    int height = 0;

    int bearingX = 0;
    int bearingY = 0;

    bool IsValid() const
    {
        return width > 0 && height > 0;
    }
    float GetU0(int atlasWidth) const
    {
        return static_cast<float>(x) /
            static_cast<float>(atlasWidth);
    }

    float GetV0(int atlasHeight) const
    {
        return static_cast<float>(y) /
            static_cast<float>(atlasHeight);
    }

    float GetU1(int atlasWidth) const
    {
        return static_cast<float>(x + width) /
            static_cast<float>(atlasWidth);
    }

    float GetV1(int atlasHeight) const
    {
        return static_cast<float>(y + height) /
            static_cast<float>(atlasHeight);
    }
};

class TextureAtlas
{
public:
    TextureAtlas(
        int width,
        int height,
        int padding = 1
    );

    bool AddGlyph(
        uint16_t glyphIndex,
        const GlyphBitmap& bitmap
    );

    const AtlasRegion* FindGlyph(
        uint16_t glyphIndex
    ) const;

    void Clear();

    int GetWidth() const
    {
        return m_width;
    }

    int GetHeight() const
    {
        return m_height;
    }

    const std::vector<uint8_t>& GetPixels() const
    {
        return m_pixels;
    }

    uint32_t GetRevision() const
    {
        return m_revision;
    }
private:
    bool AllocateRegion(
        int width,
        int height,
        int& outX,
        int& outY
    );

    void CopyBitmap(
        const GlyphBitmap& bitmap,
        int destinationX,
        int destinationY
    );

private:
    int m_width = 0;
    int m_height = 0;
    int m_padding = 1;

    std::vector<uint8_t> m_pixels;

    std::unordered_map<
        uint16_t,
        AtlasRegion
    > m_regions;
    
    // ŠÈˆÕ“I‚È’I•ûŽ®
    int m_cursorX = 0;
    int m_cursorY = 0;
    int m_currentRowHeight = 0;
    uint32_t m_revision = 0;

};