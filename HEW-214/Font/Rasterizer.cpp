#include "Rasterizer.h"
#include <algorithm>
#include <cmath>
#include <vector>

GlyphBitmap Rasterizer::RasterizeGlyph(const Glyph& glyph,
                                       uint16_t unitsPerEm, int pixelSize)
{
    GlyphBitmap bitmap;
    if (glyph.IsEmpty() || glyph.outline.contours.empty() ||
        unitsPerEm == 0 || pixelSize <= 0) return bitmap;

    const float scale = static_cast<float>(pixelSize) / unitsPerEm;
    const int pixelXMin = static_cast<int>(std::floor(glyph.xMin * scale));
    const int pixelYMin = static_cast<int>(std::floor(glyph.yMin * scale));
    const int pixelXMax = static_cast<int>(std::ceil(glyph.xMax * scale));
    const int pixelYMax = static_cast<int>(std::ceil(glyph.yMax * scale));
    bitmap.width = pixelXMax - pixelXMin;
    bitmap.height = pixelYMax - pixelYMin;
    if (bitmap.width <= 0 || bitmap.height <= 0) return GlyphBitmap{};
    bitmap.bearingX = pixelXMin;
    bitmap.bearingY = pixelYMax;
    bitmap.pixels.assign(static_cast<size_t>(bitmap.width * bitmap.height), 0);

    // Subpixel coverage preserves narrow strokes, including the lower stem of 4.
    constexpr int samples = 4;
    for (int y = 0; y < bitmap.height; ++y) {
        std::vector<int> coverage(bitmap.width, 0);
        for (int subY = 0; subY < samples; ++subY) {
            const float scanY = y + (subY + 0.5f) / samples;
            std::vector<float> intersections;
            for (const auto& contour : glyph.outline.contours) {
                if (contour.size() < 2) continue;
                for (size_t i = 0; i < contour.size(); ++i) {
                    const Vec2& a = contour[i];
                    const Vec2& b = contour[(i + 1) % contour.size()];
                    const float x0 = a.x * scale - pixelXMin;
                    const float y0 = pixelYMax - a.y * scale;
                    const float x1 = b.x * scale - pixelXMin;
                    const float y1 = pixelYMax - b.y * scale;
                    if (y0 == y1 || scanY < std::min(y0, y1) ||
                        scanY >= std::max(y0, y1)) continue;
                    const float t = (scanY - y0) / (y1 - y0);
                    intersections.push_back(x0 + (x1 - x0) * t);
                }
            }
            std::sort(intersections.begin(), intersections.end());
            for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
                const float left = intersections[i];
                const float right = intersections[i + 1];
                const int startX = std::max(0, static_cast<int>(std::floor(left)));
                const int endX = std::min(bitmap.width - 1,
                    static_cast<int>(std::ceil(right)) - 1);
                for (int x = startX; x <= endX; ++x)
                    for (int subX = 0; subX < samples; ++subX) {
                        const float sampleX = x + (subX + 0.5f) / samples;
                        if (sampleX >= left && sampleX < right) ++coverage[x];
                    }
            }
        }
        for (int x = 0; x < bitmap.width; ++x)
            SetPixel(bitmap, x, y,
                static_cast<uint8_t>((coverage[x] * 255 + 8) / (samples * samples)));
    }
    return bitmap;
}

void Rasterizer::SetPixel(GlyphBitmap& bitmap, int x, int y, uint8_t value)
{
    if (x < 0 || x >= bitmap.width || y < 0 || y >= bitmap.height) return;
    bitmap.pixels[static_cast<size_t>(y * bitmap.width + x)] = value;
}
