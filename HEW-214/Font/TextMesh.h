#pragma once

#include <cstdint>
#include <vector>

struct TextVertex
{
    float x = 0.0f;
    float y = 0.0f;

    float u = 0.0f;
    float v = 0.0f;
};

struct TextMesh
{
    std::vector<TextVertex> vertices;
    std::vector<uint32_t> indices;

    bool IsValid() const
    {
        return !vertices.empty() &&
            !indices.empty();
    }

    void Clear()
    {
        vertices.clear();
        indices.clear();
    }
};