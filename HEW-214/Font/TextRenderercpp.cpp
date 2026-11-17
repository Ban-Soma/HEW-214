#include "TextRenderer.h"

#include "Font.h"
#include "Utf8Decoder.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace
{
    struct RenderedGlyph
    {
        GlyphBitmap bitmap;

        int penX = 0;
        int advance = 0;
    };
}

TextBitmap TextRenderer::RenderText(
    const Font& font,
    const std::u32string& text)
{
    TextBitmap result;

    if (!font.IsLoaded() || text.empty())
        return result;

    const TTFParser& parser =
        font.GetParser();

    const int pixelSize =
        font.GetPixelSize();

    const uint16_t unitsPerEm =
        parser.GetUnitsPerEm();

    if (unitsPerEm == 0 || pixelSize <= 0)
        return result;

    const float scale =
        static_cast<float>(pixelSize) /
        static_cast<float>(unitsPerEm);

    const int ascentPixels =
        static_cast<int>(
            std::ceil(parser.GetAscent() * scale)
            );

    const int descentPixels =
        static_cast<int>(
            std::floor(parser.GetDescent() * scale)
            );

    result.height =
        ascentPixels - descentPixels;

    if (result.height <= 0)
        return TextBitmap{};

    const int baselineY = ascentPixels;

    struct RenderedGlyph
    {
        const GlyphBitmap* bitmap = nullptr;
        int penX = 0;
        int advance = 0;
    };

    std::vector<RenderedGlyph> renderedGlyphs;

    int penX = 0;
    int minimumX = 0;
    int maximumX = 0;

    for (char32_t codePoint : text)
    {
        uint16_t glyphIndex =
            parser.GetGlyphIndex(
                static_cast<uint32_t>(codePoint)
            );

        const HorizontalMetric* metric =
            parser.GetHorizontalMetric(
                glyphIndex
            );

        if (!metric)
            continue;

        const GlyphBitmap* glyphBitmap =
            font.GetGlyphBitmap(
                glyphIndex
            );

        const int advancePixels =
            static_cast<int>(
                std::round(
                    metric->advanceWidth * scale
                )
                );

        RenderedGlyph rendered;
        rendered.bitmap = glyphBitmap;
        rendered.penX = penX;
        rendered.advance = advancePixels;

        if (glyphBitmap &&
            glyphBitmap->IsValid())
        {
            int glyphLeft =
                penX + glyphBitmap->bearingX;

            int glyphRight =
                glyphLeft + glyphBitmap->width;

            minimumX =
                std::min(minimumX, glyphLeft);

            maximumX =
                std::max(maximumX, glyphRight);
        }

        renderedGlyphs.push_back(rendered);

        penX += advancePixels;

        maximumX =
            std::max(maximumX, penX);
    }

    result.width =
        maximumX - minimumX;

    if (result.width <= 0)
        return TextBitmap{};

    result.pixels.assign(
        static_cast<size_t>(
            result.width * result.height
            ),
        0
    );

    int horizontalOffset =
        -minimumX;

    for (const RenderedGlyph& rendered :
        renderedGlyphs)
    {
        if (!rendered.bitmap ||
            !rendered.bitmap->IsValid())
        {
            continue;
        }

        int destinationX =
            horizontalOffset +
            rendered.penX +
            rendered.bitmap->bearingX;

        int destinationY =
            baselineY -
            rendered.bitmap->bearingY;

        BlitGlyph(
            result,
            *rendered.bitmap,
            destinationX,
            destinationY
        );
    }

    return result;
}

void TextRenderer::BlitGlyph(
    TextBitmap& destination,
    const GlyphBitmap& source,
    int destinationX,
    int destinationY)
{
    for (int y = 0; y < source.height; y++)
    {
        for (int x = 0; x < source.width; x++)
        {
            const int targetX =
                destinationX + x;

            const int targetY =
                destinationY + y;

            if (targetX < 0 ||
                targetX >= destination.width ||
                targetY < 0 ||
                targetY >= destination.height)
            {
                continue;
            }

            const uint8_t sourceValue =
                source.pixels[
                    static_cast<size_t>(
                        y * source.width + x
                        )
                ];

            uint8_t& destinationValue =
                destination.pixels[
                    static_cast<size_t>(
                        targetY * destination.width +
                        targetX
                        )
                ];

            /*
                åªç›ÇÕîíçïâÊëúÇæÇ™ÅA
                maxÇ…ÇµÇƒÇ®ÇØÇŒè´óàÇÃäKí≤Ç…Ç‡ëŒâûÇµÇ‚Ç∑Ç¢ÅB
            */
            destinationValue =
                std::max(
                    destinationValue,
                    sourceValue
                );
        }
    }
}

TextBitmap TextRenderer::RenderTextUtf8(
    const Font& font,
    const std::string& utf8Text)
{
    std::u32string decodedText;

    if (!Utf8Decoder::Decode(
        utf8Text,
        decodedText))
    {
        return TextBitmap{};
    }

    return RenderText(font, decodedText);
}