#pragma once

#include "TextBitmap.h"
#include "GlyphBitmap.h"

#include <string>



class Font;

class TextRenderer
{
public:
    static TextBitmap RenderText(
        const Font& font,
        const std::u32string& text);

    static TextBitmap RenderTextUtf8(
        const Font& font,
        const std::string& utf8Text);

private:
    static void BlitGlyph(
        TextBitmap& destination,
        const GlyphBitmap& source,
        int destinationX,
        int destinationY);
};