#pragma once

#include "GlyphBitmap.h"

#include <cstdint>
#include <unordered_map>

struct GlyphCacheKey
{
    uint16_t glyphIndex = 0;
    int pixelSize = 0;

    bool operator==(const GlyphCacheKey& other) const
    {
        return glyphIndex == other.glyphIndex &&
            pixelSize == other.pixelSize;
    }
};

struct GlyphCacheKeyHash
{
    size_t operator()(const GlyphCacheKey& key) const
    {
        const size_t glyphHash =
            std::hash<uint16_t>{}(key.glyphIndex);

        const size_t sizeHash =
            std::hash<int>{}(key.pixelSize);

        return glyphHash ^
            (sizeHash << 1);
    }
};

class GlyphCache
{
public:
    const GlyphBitmap* Find(
        uint16_t glyphIndex,
        int pixelSize) const;

    void Store(
        uint16_t glyphIndex,
        int pixelSize,
        GlyphBitmap bitmap);

    void Clear();

    size_t Size() const
    {
        return m_cache.size();
    }

private:
    std::unordered_map<
        GlyphCacheKey,
        GlyphBitmap,
        GlyphCacheKeyHash
    > m_cache;
};