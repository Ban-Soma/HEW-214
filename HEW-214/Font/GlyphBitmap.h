#pragma once

#include <cstdint>
#include <vector>

struct GlyphBitmap
{
    int width = 0;
    int height = 0;

    // •`‰æ‚ÌŠî€ˆÊ’u’²®‚Ég‚¤
    int bearingX = 0;
    int bearingY = 0;

    std::vector<uint8_t> pixels;

    bool IsValid() const
    {
        return width > 0 &&
            height > 0 &&
            pixels.size() ==
            static_cast<size_t>(width * height);
    }
};