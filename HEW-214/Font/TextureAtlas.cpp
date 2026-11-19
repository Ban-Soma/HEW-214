#include "TextureAtlas.h"

#include <algorithm>

TextureAtlas::TextureAtlas(
    int width,
    int height,
    int padding)
    : m_width(width),
    m_height(height),
    m_padding(std::max(0, padding))
{
    if (m_width > 0 && m_height > 0)
    {
        m_pixels.assign(
            static_cast<size_t>(
                m_width * m_height
                ),
            0
        );
    }
}
bool TextureAtlas::AllocateRegion(
    int width,
    int height,
    int& outX,
    int& outY)
{
    if (width <= 0 || height <= 0)
        return false;

    const int requiredWidth =
        width + m_padding * 2;

    const int requiredHeight =
        height + m_padding * 2;

    if (requiredWidth > m_width ||
        requiredHeight > m_height)
    {
        return false;
    }

    /*
        現在の行に入らない場合、
        次の行へ移動する。
    */
    if (m_cursorX + requiredWidth > m_width)
    {
        m_cursorX = 0;
        m_cursorY += m_currentRowHeight;
        m_currentRowHeight = 0;
    }

    if (m_cursorY + requiredHeight > m_height)
        return false;

    outX = m_cursorX + m_padding;
    outY = m_cursorY + m_padding;

    m_cursorX += requiredWidth;

    m_currentRowHeight =
        std::max(
            m_currentRowHeight,
            requiredHeight
        );

    return true;
}
void TextureAtlas::CopyBitmap(
    const GlyphBitmap& bitmap,
    int destinationX,
    int destinationY)
{
    for (int y = 0; y < bitmap.height; y++)
    {
        for (int x = 0; x < bitmap.width; x++)
        {
            const int atlasX =
                destinationX + x;

            const int atlasY =
                destinationY + y;

            if (atlasX < 0 ||
                atlasX >= m_width ||
                atlasY < 0 ||
                atlasY >= m_height)
            {
                continue;
            }

            const uint8_t source =
                bitmap.pixels[
                    static_cast<size_t>(
                        y * bitmap.width + x
                        )
                ];

            m_pixels[
                static_cast<size_t>(
                    atlasY * m_width + atlasX
                    )
            ] = source;
        }
    }
}
bool TextureAtlas::AddGlyph(
    uint16_t glyphIndex,
    const GlyphBitmap& bitmap)
{
    if (!bitmap.IsValid())
        return false;

    /*
        すでに登録済みなら成功扱い。
    */
    if (m_regions.find(glyphIndex) !=
        m_regions.end())
    {
        return true;
    }

    int atlasX = 0;
    int atlasY = 0;

    if (!AllocateRegion(
        bitmap.width,
        bitmap.height,
        atlasX,
        atlasY))
    {
        return false;
    }

    CopyBitmap(
        bitmap,
        atlasX,
        atlasY
    );

    AtlasRegion region;

    region.x = atlasX;
    region.y = atlasY;
    region.width = bitmap.width;
    region.height = bitmap.height;
    region.bearingX = bitmap.bearingX;
    region.bearingY = bitmap.bearingY;

    m_regions.emplace(
        glyphIndex,
        region
    );
    m_revision++;
    return true;
}
const AtlasRegion*
TextureAtlas::FindGlyph(
    uint16_t glyphIndex) const
{
    auto it =
        m_regions.find(glyphIndex);

    if (it == m_regions.end())
        return nullptr;

    return &it->second;
}

void TextureAtlas::Clear()
{
    std::fill(
        m_pixels.begin(),
        m_pixels.end(),
        0
    );

    m_regions.clear();

    m_cursorX = 0;
    m_cursorY = 0;
    m_currentRowHeight = 0;

    m_revision++;
}