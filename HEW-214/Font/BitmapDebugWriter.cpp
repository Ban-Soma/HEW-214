#include "BitmapDebugWriter.h"

#include <fstream>

bool SaveGlyphBitmapPGM(
    const GlyphBitmap& bitmap,
    const std::string& filename)
{
    if (!bitmap.IsValid())
        return false;

    std::ofstream file(filename, std::ios::binary);

    if (!file)
        return false;

    file << "P5\n"
        << bitmap.width << " "
        << bitmap.height << "\n255\n";

    file.write(
        reinterpret_cast<const char*>(bitmap.pixels.data()),
        static_cast<std::streamsize>(bitmap.pixels.size())
    );

    return file.good();
}
bool SaveTextBitmapPGM(
    const TextBitmap& bitmap,
    const std::string& filename)
{
    if (!bitmap.IsValid())
        return false;

    std::ofstream file(
        filename,
        std::ios::binary
    );

    if (!file)
        return false;

    file << "P5\n"
        << bitmap.width << " "
        << bitmap.height
        << "\n255\n";

    file.write(
        reinterpret_cast<const char*>(
            bitmap.pixels.data()
            ),
        static_cast<std::streamsize>(
            bitmap.pixels.size()
            )
    );

    return file.good();
}

bool SaveTextureAtlasPGM(
    const TextureAtlas& atlas,
    const std::string& filename)
{
    if (atlas.GetWidth() <= 0 ||
        atlas.GetHeight() <= 0)
    {
        return false;
    }

    std::ofstream file(
        filename,
        std::ios::binary
    );

    if (!file)
        return false;

    file << "P5\n"
        << atlas.GetWidth() << " "
        << atlas.GetHeight()
        << "\n255\n";

    const auto& pixels =
        atlas.GetPixels();

    file.write(
        reinterpret_cast<const char*>(
            pixels.data()
            ),
        static_cast<std::streamsize>(
            pixels.size()
            )
    );

    return file.good();
}