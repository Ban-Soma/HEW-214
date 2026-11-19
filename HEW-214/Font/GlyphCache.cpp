
#include "GlyphCache.h"

#include <utility>

const GlyphBitmap* GlyphCache::Find(
    uint16_t glyphIndex,
    int pixelSize) const
{
    GlyphCacheKey key{
        glyphIndex,
        pixelSize
    };

    auto it = m_cache.find(key);

    if (it == m_cache.end())
        return nullptr;

    return &it->second;
}

void GlyphCache::Store(
    uint16_t glyphIndex,
    int pixelSize,
    GlyphBitmap bitmap)
{
    GlyphCacheKey key{
        glyphIndex,
        pixelSize
    };

    m_cache.insert_or_assign(
        key,
        std::move(bitmap)
    );
}

void GlyphCache::Clear()
{
    m_cache.clear();
}