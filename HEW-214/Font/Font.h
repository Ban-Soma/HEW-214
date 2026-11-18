#pragma once

#include "TTFParser.h"
#include "GlyphCache.h"
#include "TextBitmap.h"
#include "TextureAtlas.h"
#include "TextMesh.h"
#include "IFontRenderer.h"
#include <string>

class Font
{
public:
    Font() = default;

    Font(
        const std::string& filename,
        int pixelSize
    );

    bool Load(
        const std::string& filename,
        int pixelSize
    );

    bool IsLoaded() const
    {
        return m_isLoaded;
    }

    int GetPixelSize() const
    {
        return m_pixelSize;
    }
    size_t GetGlyphCacheSize() const
    {
        return m_glyphCache.Size();
    }

    void SetPixelSize(int pixelSize);

    TextBitmap RenderTextUtf8(
        const std::string& text
    ) const;

    TextBitmap RenderText(
        const std::u32string& text
    ) const;
    const TTFParser& GetParser() const
    {
        return m_parser;
    }

    const GlyphBitmap* GetGlyphBitmap(
        uint16_t glyphIndex) const;
    TextMesh BuildTextMeshUtf8(
        const std::string& text,
        float x,
        float y) const;

    const TextureAtlas& GetTextureAtlas() const
    {
        return m_atlas;
    }

    bool DrawText(IFontRenderer& renderer,const std::string& utf8Text,float x,float y) const;
    const AtlasRegion* GetOrCreateAtlasRegion(
        uint16_t glyphIndex) const;

private:
    TTFParser m_parser;
    mutable GlyphCache m_glyphCache;
    int m_pixelSize = 0;
    bool m_isLoaded = false;
    mutable TextureAtlas m_atlas{ 1024, 1024, 1 };

    
    
};
