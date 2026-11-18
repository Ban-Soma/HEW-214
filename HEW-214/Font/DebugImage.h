#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct DebugImage
{
    int width = 512;
    int height = 512;
    std::vector<uint8_t> pixels;

    DebugImage(int w, int h);

    void Clear(uint8_t value = 255);
    void SetPixel(int x, int y, uint8_t value);
    void DrawLine(int x0, int y0, int x1, int y1);
    bool SavePPM(const std::string& filename) const;
};