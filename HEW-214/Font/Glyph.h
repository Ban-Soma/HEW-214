#pragma once
#include <cstdint>
#include <vector>




struct GlyphPoint
{
    int16_t x = 0;
    int16_t y = 0;
    bool onCurve = false;
};

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

struct GlyphContour
{
    std::vector<GlyphPoint> points;
};

struct GlyphOutline
{
    std::vector<std::vector<Vec2>> contours;
};

enum class GlyphSegmentType
{
    Line,
    Quadratic
};

struct GlyphSegment
{
    GlyphSegmentType type = GlyphSegmentType::Line;

    Vec2 start;
    Vec2 control;
    Vec2 end;
};

struct GlyphSegmentContour
{
    std::vector<GlyphSegment> segments;
};

struct Glyph
{
    bool empty = true;
    int16_t numberOfContours = 0;

    int16_t xMin = 0;
    int16_t yMin = 0;
    int16_t xMax = 0;
    int16_t yMax = 0;

    std::vector<uint16_t> endPtsOfContours;
    uint16_t instructionLength = 0;
    std::vector<uint8_t> instructions;

    std::vector<uint8_t> flags;
    std::vector<GlyphPoint> points;

    std::vector<GlyphContour> contours;
    GlyphOutline outline;

    std::vector<GlyphSegmentContour> segmentContours;

    bool IsEmpty() const { return empty; }
    bool IsComposite() const { return !empty&&numberOfContours<0; }
    bool IsSimple() const { return !empty&&numberOfContours>=0; }
};