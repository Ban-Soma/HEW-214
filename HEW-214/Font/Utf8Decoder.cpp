#include "Utf8Decoder.h"

bool Utf8Decoder::Decode(const std::string& utf8,std::u32string& outText)
{
    outText.clear();

    size_t i = 0;

    while (i < utf8.size())
    {
        const uint8_t first =
            static_cast<uint8_t>(utf8[i]);

        char32_t codePoint = 0;
        size_t sequenceLength = 0;

        // 1バイト: ASCII
        if ((first & 0x80) == 0)
        {
            codePoint = first;
            sequenceLength = 1;
        }
        // 2バイト
        else if ((first & 0xE0) == 0xC0)
        {
            codePoint = first & 0x1F;
            sequenceLength = 2;
        }
        // 3バイト
        else if ((first & 0xF0) == 0xE0)
        {
            codePoint = first & 0x0F;
            sequenceLength = 3;
        }
        // 4バイト
        else if ((first & 0xF8) == 0xF0)
        {
            codePoint = first & 0x07;
            sequenceLength = 4;
        }
        else
        {
            return false;
        }

        if (i + sequenceLength > utf8.size())
            return false;

        for (size_t j = 1; j < sequenceLength; j++)
        {
            const uint8_t continuation =
                static_cast<uint8_t>(utf8[i + j]);

            // 継続バイトは10xxxxxxでなければならない
            if ((continuation & 0xC0) != 0x80)
                return false;

            codePoint =
                (codePoint << 6) |
                (continuation & 0x3F);
        }

        // 過長エンコーディングを拒否
        if (sequenceLength == 2 &&
            codePoint < 0x80)
        {
            return false;
        }

        if (sequenceLength == 3 &&
            codePoint < 0x800)
        {
            return false;
        }

        if (sequenceLength == 4 &&
            codePoint < 0x10000)
        {
            return false;
        }

        // Unicodeの範囲外
        if (codePoint > 0x10FFFF)
            return false;

        // UTF-16サロゲート領域はUnicode文字ではない
        if (codePoint >= 0xD800 &&
            codePoint <= 0xDFFF)
        {
            return false;
        }

        outText.push_back(codePoint);
        i += sequenceLength;
    }

    return true;
}