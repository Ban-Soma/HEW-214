#include "Font.h"
#include "Rasterizer.h"
#include "TextRenderer.h"
#include "Utf8Decoder.h"

#include <cmath>
Font::Font(
    const std::string& filename,
    int pixelSize)
{
    Load(filename, pixelSize);
}

bool Font::Load(
    const std::string& filename,
    int pixelSize)
{
    m_isLoaded = false;
    m_pixelSize = 0;
    m_glyphCache.Clear();

    if (pixelSize <= 0)
        return false;

    if (!m_parser.Load(filename))
        return false;

    m_pixelSize = pixelSize;
    m_isLoaded = true;

    return true;
}

void Font::SetPixelSize(int pixelSize)
{
    if (pixelSize <= 0)
        return;
    
    if (m_pixelSize == pixelSize)
        return;
    m_pixelSize = pixelSize;
    m_glyphCache.Clear();
}

TextBitmap Font::RenderTextUtf8(
    const std::string& text) const
{
    if (!m_isLoaded)
        return TextBitmap{};

    return TextRenderer::RenderTextUtf8(
        *this,text
    );
}

TextBitmap Font::RenderText(
    const std::u32string& text) const
{
    if (!m_isLoaded)
        return TextBitmap{};

    return TextRenderer::RenderText(
        *this ,text
    );
}
#include "Rasterizer.h"

const GlyphBitmap* Font::GetGlyphBitmap(
    uint16_t glyphIndex) const
{
    if (!m_isLoaded)
        return nullptr;

    if (const GlyphBitmap* cached =
        m_glyphCache.Find(
            glyphIndex,
            m_pixelSize))
    {
        return cached;
    }

    Glyph glyph;

    if (!m_parser.ReadGlyph(
        glyphIndex,
        glyph))
    {
        return nullptr;
    }

    GlyphBitmap bitmap =
        Rasterizer::RasterizeGlyph(
            glyph,
            m_parser.GetUnitsPerEm(),
            m_pixelSize
        );

    m_glyphCache.Store(
        glyphIndex,
        m_pixelSize,
        std::move(bitmap)
    );

    return m_glyphCache.Find(
        glyphIndex,
        m_pixelSize
    );
}
const AtlasRegion* Font::GetOrCreateAtlasRegion(
    uint16_t glyphIndex) const
{
    if (const AtlasRegion* existing =
        m_atlas.FindGlyph(glyphIndex))
    {
        return existing;
    }

    const GlyphBitmap* bitmap =
        GetGlyphBitmap(glyphIndex);

    if (!bitmap || !bitmap->IsValid())
        return nullptr;

    if (!m_atlas.AddGlyph(
        glyphIndex,
        *bitmap))
    {
        return nullptr;
    }

    return m_atlas.FindGlyph(glyphIndex);
}

TextMesh Font::BuildTextMeshUtf8(
    const std::string& text,
    float x,
    float y) const
{
    TextMesh mesh;

    if (!m_isLoaded || text.empty())
        return mesh;

    std::u32string decoded;

    if (!Utf8Decoder::Decode(text, decoded))
        return mesh;

    const float scale =
        static_cast<float>(m_pixelSize) /
        static_cast<float>(
            m_parser.GetUnitsPerEm()
            );

    float penX = x;

    for (char32_t codePoint : decoded)
    {
        const uint16_t glyphIndex =
            m_parser.GetGlyphIndex(
                static_cast<uint32_t>(codePoint)
            );

        const HorizontalMetric* metric =
            m_parser.GetHorizontalMetric(
                glyphIndex
            );

        if (!metric)
            continue;

        const AtlasRegion* region =
            GetOrCreateAtlasRegion(
                glyphIndex
            );

        if (region && region->IsValid())
        {
            const float left =
                penX +
                static_cast<float>(
                    region->bearingX
                    );

            const float top =
                y -
                static_cast<float>(
                    region->bearingY
                    );

            const float right =
                left +
                static_cast<float>(
                    region->width
                    );

            const float bottom =
                top +
                static_cast<float>(
                    region->height
                    );

            const float u0 =
                region->GetU0(
                    m_atlas.GetWidth()
                );

            const float v0 =
                region->GetV0(
                    m_atlas.GetHeight()
                );

            const float u1 =
                region->GetU1(
                    m_atlas.GetWidth()
                );

            const float v1 =
                region->GetV1(
                    m_atlas.GetHeight()
                );

            const uint32_t baseIndex =
                static_cast<uint32_t>(
                    mesh.vertices.size()
                    );

            mesh.vertices.push_back({
                left, top, u0, v0
                });

            mesh.vertices.push_back({
                right, top, u1, v0
                });

            mesh.vertices.push_back({
                right, bottom, u1, v1
                });

            mesh.vertices.push_back({
                left, bottom, u0, v1
                });

            mesh.indices.push_back(
                baseIndex + 0
            );

            mesh.indices.push_back(
                baseIndex + 1
            );

            mesh.indices.push_back(
                baseIndex + 2
            );

            mesh.indices.push_back(
                baseIndex + 0
            );

            mesh.indices.push_back(
                baseIndex + 2
            );

            mesh.indices.push_back(
                baseIndex + 3
            );
        }

        const float advance =
            static_cast<float>(
                metric->advanceWidth
                ) * scale;

        penX += advance;
    }

    return mesh;
}

bool Font::DrawText(IFontRenderer& renderer, const std::string& utf8Text, float x, float y) const
{
    if (!m_isLoaded)
        return false;

    TextMesh mesh =
        BuildTextMeshUtf8(
            utf8Text,
            x,
            y
        );

    if (!mesh.IsValid())
        return false;

    renderer.DrawTextMesh(
        mesh,
        m_atlas
    );

    return true;
}
