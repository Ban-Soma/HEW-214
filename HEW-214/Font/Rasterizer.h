#pragma once

#include "Glyph.h"
#include "GlyphBitmap.h"
class Rasterizer
{
public:
    static GlyphBitmap RasterizeGlyph(
        const Glyph& glyph,
        uint16_t unitsPerEm,
        int pixelSize);

private:
    static void SetPixel(
        GlyphBitmap& bitmap,
        int x,
        int y,
        uint8_t value);
};
