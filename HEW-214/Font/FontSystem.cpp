#include "FontSystem.h"

#include "AtlasTexture.h"
#include "SpriteFontRenderer.h"
#include "Utf8Decoder.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace
{
    struct LayoutGlyph
    {
        uint16_t glyphIndex = 0;
        const AtlasRegion* region = nullptr;
        float penX = 0.0f;
        float advance = 0.0f;

    };
    struct LayoutLine
    {
        std::vector<LayoutGlyph>glyphs;
        float width = 0.0f;
    };
}
FontSystem::~FontSystem()
{
    Finalize();
}
void FontSystem::Finalize()
{
    DeleteAtlasTexture(m_atlasTexture);
    FinalizeSpriteFontRendererDX11();

    if (m_context)
    {
        m_context->Release();
        m_context = nullptr;
    }
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

    m_uploadedAtlasRevision = 0;
    m_initialized = false;
}

bool FontSystem::Initialize(ID3D11Device* device, ID3D11DeviceContext* context,
                            const std::string& fontFile, int pixelSize)
{
    Finalize();

    if (!device || !context || pixelSize <= 0)
        return false;

    m_device = device;
    m_context = context;
    m_device->AddRef();
    m_context->AddRef();

    if (!m_font.Load(fontFile, pixelSize))
    {
        Finalize();
        return false;
    }

    if (!InitializeSpriteFontRendererDX11(m_device))
    {
        Finalize();
        return false;
    }

    if (!CreateAtlasTexture(m_device, m_font.GetTextureAtlas(), m_atlasTexture))
    {
        Finalize();
        return false;
    }

    m_uploadedAtlasRevision = m_font.GetTextureAtlas().GetRevision();
    m_initialized = true;
    return true;
}

TextMesh FontSystem::BuildTextMesh(const std::string& utf8Text, float x, float y)
{
    if (!m_initialized) return TextMesh{};

    TextMesh mesh = m_font.BuildTextMeshUtf8(utf8Text, x, y);
    if (!mesh.IsValid())return TextMesh{};
    if (!UpdateAtlasIfNeeded()) return TextMesh{};

    return mesh;
}
TextMesh FontSystem::BuildTextMesh(const std::string& utf8Text,const TextLayout& layout,float& outWidth,float& outHeight)
{
    TextMesh result;

    outWidth = 0.0f;
    outHeight = 0.0f;

    if (!m_initialized || utf8Text.empty())
        return result;

    std::u32string decoded;

    if (!Utf8Decoder::Decode(utf8Text,decoded))
        return result;
    
    const TTFParser& parser =m_font.GetParser();

    const uint16_t unitsPerEm =parser.GetUnitsPerEm();

    if (unitsPerEm == 0)return result;

    const float scale = static_cast<float>(m_font.GetPixelSize()) /static_cast<float>(unitsPerEm);

    const float baseLineHeight = static_cast<float>( parser.GetAscent() - parser.GetDescent() + parser.GetLineGap() ) * scale;

    const float lineHeight =  baseLineHeight + layout.lineSpacing;

    std::vector<LayoutLine> lines;
    LayoutLine currentLine;

    for (char32_t codePoint : decoded)
    {
        // Iȉs
        if (codePoint == U'\n')
        {
            lines.push_back(   std::move(currentLine)
            );

            currentLine = LayoutLine{};
            continue;
        }

        const uint16_t glyphIndex = parser.GetGlyphIndex(  static_cast<uint32_t>( codePoint )  );

        const HorizontalMetric* metric = parser.GetHorizontalMetric(  glyphIndex  );

        if (!metric)
            continue;

        const float advance = static_cast<float>(metric->advanceWidth) * scale;

        /*
            sł͂ȂԂōő啝𒴂ȂA
            ݂̍sm肵Ď̍s֑B
        */
        if (layout.maxWidth > 0.0f && !currentLine.glyphs.empty() &&currentLine.width + advance >layout.maxWidth)
        {
            lines.push_back( std::move(currentLine));

            currentLine = LayoutLine{};
        }

        const AtlasRegion* region = m_font.GetOrCreateAtlasRegion( glyphIndex );

        LayoutGlyph glyph;
        glyph.glyphIndex = glyphIndex;
        glyph.region = region;
        glyph.penX = currentLine.width;
        glyph.advance = advance;

        currentLine.glyphs.push_back(glyph);
        currentLine.width += advance;
    }

    /*
        Ō̍sǉB
        sꍇ͋scB
    */
    if (!currentLine.glyphs.empty() ||lines.empty() ||
        (!decoded.empty() && decoded.back() == U'\n'))
    {
        lines.push_back(  std::move(currentLine)
        );
    }

    if (lines.empty())
        return result;

    float maximumLineWidth = 0.0f;

    for (const LayoutLine& line : lines)
    {
        maximumLineWidth = (std::max)( maximumLineWidth, line.width );
    }

    /*
        maxWidthw肳ĂꍇA
        ʒůmaxWidthB

        wȂŒs̕B
    */
    const float layoutWidth =layout.maxWidth > 0.0f? layout.maxWidth : maximumLineWidth;

    const float ascentPixels = static_cast<float>(parser.GetAscent() ) * scale;

    for (size_t lineIndex = 0;lineIndex < lines.size();  lineIndex++)
    {
        const LayoutLine& line =lines[lineIndex];

        float alignmentOffset = 0.0f;

        switch (layout.alignment)
        {
        case TextAlignment::Left:
            alignmentOffset = 0.0f;
            break;

        case TextAlignment::Center:
            alignmentOffset = (layoutWidth - line.width) * 0.5f;
            break;

        case TextAlignment::Right:
            alignmentOffset =layoutWidth - line.width;
            break;
        }

        const float baselineY = static_cast<float>(lineIndex) * lineHeight;

        for (const LayoutGlyph& glyph :line.glyphs)
        {
            const AtlasRegion* region = glyph.region;

            if (!region ||  !region->IsValid())
            {
                continue;
            }

            const float left = alignmentOffset + glyph.penX +
                static_cast<float>(region->bearingX );

            const float top =baselineY -static_cast<float>(  region->bearingY ) +
                ascentPixels;

            const float right = left +
                static_cast<float>( region->width );

            const float bottom =top +
                static_cast<float>( region->height );

            const TextureAtlas& atlas = m_font.GetTextureAtlas();

            const float u0 =
                region->GetU0(  atlas.GetWidth()
                );

            const float v0 =
                region->GetV0( atlas.GetHeight()
                );

            const float u1 =
                region->GetU1( atlas.GetWidth()
                );

            const float v1 =
                region->GetV1( atlas.GetHeight()
                );

            const uint32_t baseIndex = static_cast<uint32_t>( result.vertices.size()  );

            result.vertices.push_back({
                left, top, u0, v0
                });

            result.vertices.push_back({
                right, top, u1, v0
                });

            result.vertices.push_back({
                right, bottom, u1, v1
                });

            result.vertices.push_back({
                left, bottom, u0, v1
                });

            result.indices.insert(
                result.indices.end(),
                {
                    baseIndex + 0,
                    baseIndex + 1,
                    baseIndex + 2,

                    baseIndex + 0,
                    baseIndex + 2,
                    baseIndex + 3
                }
            );
        }
    }

    outWidth = layout.maxWidth > 0.0f ? layout.maxWidth : maximumLineWidth;

    outHeight = baseLineHeight +static_cast<float>( lines.size() - 1) * lineHeight;

    if (!UpdateAtlasIfNeeded())
        return TextMesh{};

    return result;
}

bool FontSystem::UpdateAtlasIfNeeded()
{
    const TextureAtlas& atlas =
        m_font.GetTextureAtlas();

    if (atlas.GetRevision() ==m_uploadedAtlasRevision)
        return true;
    

    if (!UpdateAtlasTexture(m_context, m_atlasTexture, atlas))
        return false;
    

    m_uploadedAtlasRevision =atlas.GetRevision();

    return true;
}

void FontSystem::DrawMesh(const TextMesh& mesh,float x,float y, float  r, float g, float b, float a)
{
    if (!m_initialized)return;
    if (!m_atlasTexture.IsValid())return;
    if (!mesh.IsValid())return;
    DrawTextMeshWithSpriteDX11(m_context, mesh, m_atlasTexture.srv, x, y, r, g, b, a);
}