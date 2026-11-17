#include "DebugImage.h"
#include <fstream>
#include <cmath>

DebugImage::DebugImage(int w, int h)
    : width(w), height(h), pixels(w* h, 255)
{
}

void DebugImage::Clear(uint8_t value)
{
    std::fill(pixels.begin(), pixels.end(), value);
}

void DebugImage::SetPixel(int x, int y, uint8_t value)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    pixels[y * width + x] = value;
}

void DebugImage::DrawLine(int x0, int y0, int x1, int y1)
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true)
    {
        SetPixel(x0, y0, 0);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err * 2;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

bool DebugImage::SavePPM(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return false;

    file << "P5\n" << width << " " << height << "\n255\n";
    file.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

    return true;
}