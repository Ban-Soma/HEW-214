#include "TTFParser.h"
#include "Tag.h"
#include <fstream>
#include <iostream>
#include "BinaryReader.h"
#include "Glyph.h"
#include <algorithm>
#include <utility>

bool TTFParser::Load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        return false;
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);
    m_data.resize(size);
    file.read(reinterpret_cast<char*>(m_data.data()), size);

    //m_reader = new BinaryReader(m_data);
    m_reader = std::make_unique<BinaryReader>(m_data);
    m_tables.clear();
    m_sfntOffset = 0;
    if (m_data.size() >= 16 && m_reader->ReadUInt32(0) == 0x74746366) {
        // TrueType collection: use the first sfnt; table offsets remain file absolute.
        m_sfntOffset = m_reader->ReadUInt32(12);
        if (m_sfntOffset > m_data.size() - 12) return false;
    }
    return ReadOffsetTable() && ReadTableDirectory()&&
        ReadHeadTable()&&ReadMaxpTable()&&
        ReadLocaTable()&& ReadCmapTable()&&
        ReadHheaTable() && ReadHmtxTable();
}

const TableRecord* TTFParser::FindTable(uint32_t tag) const
{
    auto it = m_tables.find(tag);

    if (it == m_tables.end())
        return nullptr;

    return &it->second;
}

bool TTFParser::ReadOffsetTable()
{
    uint32_t version = m_reader->ReadUInt32(m_sfntOffset);

    if (version != 0x00010000)
    {
        std::cout << "Not TrueType\n";
        return false;
    }

    m_numTables = m_reader->ReadUInt16(m_sfntOffset + 4);


    return true;
}

bool TTFParser::ReadTableDirectory()
{
    size_t offset = m_sfntOffset + 12;

    for (uint16_t i = 0; i < m_numTables; i++)
    {
        TableRecord record;

        record.tag = m_reader->ReadUInt32(offset);
        record.checkSum = m_reader->ReadUInt32(offset + 4);
        record.offset = m_reader->ReadUInt32(offset + 8);
        record.length = m_reader->ReadUInt32(offset + 12);

        m_tables.emplace(record.tag, record);
        
        //m_tables[record.tag] = record;

        ////std::cout
        ////    << record.tag
        ////    << " offset="
        ////    << record.offset
        ////    << " length="
        ////    << record.length
        ////    << std::endl;
        char tag[5];

        tag[0] = char(record.tag >> 24);
        tag[1] = char(record.tag >> 16);
        tag[2] = char(record.tag >> 8);
        tag[3] = char(record.tag);
        tag[4] = '\0';
        if (record.tag == Tag::MAXP)
        {
            
            uint32_t off = record.offset;
            uint32_t len = record.length;
            int a = 0;
        }
        if (i == 2)
        {

        }
        offset += 16;

    }
    int a = m_tables.size();

    return true;
}

bool TTFParser::ReadHeadTable()
{
    auto table = FindTable(Tag::HEAD);
    if(!table)
        return false;

    size_t o = table->offset;
    m_tablesData.head.unitsPerEm = m_reader->ReadUInt16(o + 18);

    m_tablesData.head.xMin = m_reader->ReadInt16(o + 36);
    m_tablesData.head.yMin = m_reader->ReadInt16(o + 38);
    m_tablesData.head.xMax = m_reader->ReadInt16(o + 40);
    m_tablesData.head.yMax = m_reader->ReadInt16(o + 42);

    m_tablesData.head.indexToLocFormat = m_reader->ReadInt16(o + 50);

    return true;


}

//bool TTFParser::ReadMaxpTable()
//{
//    const TableRecord* table = FindTable(Tag::MAXP);
//    if(!table)
//        return false;
//    size_t o = table->offset;
//
//    m_tablesData.maxp.version = m_reader->ReadUInt32(o);
//    m_tablesData.maxp.numGlyphs = m_reader->ReadUInt16(o+4);
//
//    uint8_t b0 = m_data[o + 0];
//    uint8_t b1 = m_data[o + 1];
//    uint8_t b2 = m_data[o + 2];
//    uint8_t b3 = m_data[o + 3];
//    return true;
//}
bool TTFParser::ReadMaxpTable()
{
    const TableRecord* table = FindTable(Tag::MAXP);
    if (!table)
        return false;

    size_t o = table->offset;

    uint8_t b0 = m_data[o + 0];
    uint8_t b1 = m_data[o + 1];
    uint8_t b2 = m_data[o + 2];
    uint8_t b3 = m_data[o + 3];
    uint8_t b4 = m_data[o + 4];
    uint8_t b5 = m_data[o + 5];

    uint32_t version = m_reader->ReadUInt32(o);
    uint16_t glyphs = m_reader->ReadUInt16(o + 4);

    m_tablesData.maxp.version = version;
    m_tablesData.maxp.numGlyphs = glyphs;

    return true;
}
bool TTFParser::ReadLocaTable()
{
    const TableRecord* table = FindTable(Tag::LOCA);
    if(!table)
        return false;

    size_t o = table->offset;

    uint16_t numGlyphs = m_tablesData.maxp.numGlyphs;
    int16_t format = m_tablesData.head.indexToLocFormat;
    m_tablesData.loca.offsets.resize(numGlyphs + 1);

    if (format == 0)
    {
        for (uint16_t i = 0; i <= numGlyphs; i++)
        {
            uint16_t value = m_reader->ReadUInt16(o + i * 2);
            m_tablesData.loca.offsets[i] = uint32_t(value) * 2;
        }
    }
    else if (format == 1)
    {
        for (uint16_t i = 0; i <= numGlyphs; i++)
        {
            m_tablesData.loca.offsets[i] =
                m_reader->ReadInt32(o + i * 4);
        }
    }
    else
    {
        return false;
    }
    return true;
}
bool TTFParser::ReadGlyph(uint16_t glyphIndex, Glyph& outGlyph) const
{
    return ReadGlyphInternal(glyphIndex, outGlyph, 0);
}
bool TTFParser::ReadGlyphInternal(uint16_t glyphIndex, Glyph& outGlyph, int depth) const
{
    if (depth > 8) return false;
    size_t glyphDataOffset = 0;
    if (!ReadGlyphBasic(glyphIndex, outGlyph, glyphDataOffset)) return false;
    if (outGlyph.IsEmpty()) return true;
    if (outGlyph.IsSimple()) return ReadSimpleGlyph(outGlyph, glyphDataOffset);

    size_t p = glyphDataOffset + 10;
    uint16_t flags = 0;
    do {
        if (p + 4 > m_data.size()) return false;
        flags = m_reader->ReadUInt16(p);
        const uint16_t partIndex = m_reader->ReadUInt16(p + 2);
        p += 4;
        const bool words = (flags & 0x0001) != 0;
        const bool xy = (flags & 0x0002) != 0;
        if (!xy || p + (words ? 4 : 2) > m_data.size()) return false;
        int x = 0, y = 0;
        if (words) {
            x = m_reader->ReadInt16(p);
            y = m_reader->ReadInt16(p + 2);
            p += 4;
        } else {
            x = static_cast<int8_t>(m_reader->ReadUInt8(p));
            y = static_cast<int8_t>(m_reader->ReadUInt8(p + 1));
            p += 2;
        }
        float a = 1, b = 0, c = 0, d = 1;
        const auto fixed = [this](size_t at) { return m_reader->ReadInt16(at) / 16384.0f; };
        if (flags & 0x0008) {
            if (p + 2 > m_data.size()) return false;
            a = d = fixed(p); p += 2;
        } else if (flags & 0x0040) {
            if (p + 4 > m_data.size()) return false;
            a = fixed(p); d = fixed(p + 2); p += 4;
        } else if (flags & 0x0080) {
            if (p + 8 > m_data.size()) return false;
            a = fixed(p); b = fixed(p + 2);
            c = fixed(p + 4); d = fixed(p + 6); p += 8;
        }
        Glyph part;
        if (!ReadGlyphInternal(partIndex, part, depth + 1)) return false;
        for (const auto& contour : part.outline.contours) {
            std::vector<Vec2> transformed;
            transformed.reserve(contour.size());
            for (const Vec2& v : contour)
                transformed.push_back({a * v.x + b * v.y + x, c * v.x + d * v.y + y});
            outGlyph.outline.contours.push_back(std::move(transformed));
        }
    } while (flags & 0x0020);
    return !outGlyph.outline.contours.empty();
}

uint16_t TTFParser::GetGlyphIndex(uint32_t codePoint) const
{
    if (m_tablesData.cmap.format == 4)
        return GetGlyphIndexFormat4(codePoint);
    return 0;
}

bool TTFParser::ReadGlyphBasic(uint16_t glyphIndex,Glyph & outGlyph,size_t & outGlyphDataOffset) const
    {
        if (glyphIndex >= m_tablesData.maxp.numGlyphs)
            return false;

        const TableRecord* glyfTable = FindTable(Tag::GLYF);
        if (!glyfTable)
            return false;

        uint32_t glyphOffset = m_tablesData.loca.offsets[glyphIndex];
        uint32_t nextOffset = m_tablesData.loca.offsets[glyphIndex + 1];

        if (glyphOffset == nextOffset)
        {
            outGlyph = Glyph{};
            outGlyph.empty = true;
            return true;
        }

        outGlyph = Glyph{};
        outGlyph.empty = false;

        size_t o = glyfTable->offset + glyphOffset;
        outGlyphDataOffset = o;

        outGlyph.numberOfContours = m_reader->ReadInt16(o + 0);
        outGlyph.xMin = m_reader->ReadInt16(o + 2);
        outGlyph.yMin = m_reader->ReadInt16(o + 4);
        outGlyph.xMax = m_reader->ReadInt16(o + 6);
        outGlyph.yMax = m_reader->ReadInt16(o + 8);

        return true;
    }

bool TTFParser::ReadSimpleGlyph(Glyph& outGlyph, size_t glyphDataOffset) const
{
    if (outGlyph.numberOfContours == 0) return true;
    size_t p = glyphDataOffset + 10;

    outGlyph.endPtsOfContours.resize(outGlyph.numberOfContours);

    for (int i = 0; i < outGlyph.numberOfContours; i++)
    {
        outGlyph.endPtsOfContours[i] = m_reader->ReadUInt16(p);
        p += 2;
    }

    outGlyph.instructionLength = m_reader->ReadUInt16(p);
    p += 2;

    outGlyph.instructions.resize(outGlyph.instructionLength);

    for (uint16_t i = 0; i < outGlyph.instructionLength; i++)
    {
        outGlyph.instructions[i] = m_reader->ReadUInt8(p);
        p++;
    }

    uint16_t numPoints = outGlyph.endPtsOfContours.back() + 1;

    ReadGlyphFlags(outGlyph, p, numPoints);
    ReadGlyphCoordinates(outGlyph, p, numPoints);

    BuildContours(outGlyph);
    BuildSegments(outGlyph);
    FlattenSegments(outGlyph);

    return true;
}
void TTFParser::ReadGlyphFlags(Glyph& outGlyph, size_t& p, uint16_t numPoints) const
{
    outGlyph.flags.clear();
    outGlyph.flags.reserve(numPoints);

    while (outGlyph.flags.size() < numPoints)
    {
        uint8_t flag = m_reader->ReadUInt8(p);
        p++;

        outGlyph.flags.push_back(flag);

        if (flag & 0x08)
        {
            uint8_t repeatCount = m_reader->ReadUInt8(p);
            p++;

            for (uint8_t i = 0; i < repeatCount; i++)
            {
                outGlyph.flags.push_back(flag);
            }
        }
    }
}
void TTFParser::ReadGlyphCoordinates(Glyph& outGlyph, size_t& p, uint16_t numPoints) const
{
    outGlyph.points.resize(numPoints);

    int16_t currentX = 0;

    for (uint16_t i = 0; i < numPoints; i++)
    {
        uint8_t flag = outGlyph.flags[i];
        int16_t dx = 0;

        if (flag & 0x02)
        {
            uint8_t value = m_reader->ReadUInt8(p);
            p++;

            dx = (flag & 0x10) ? value : -value;
        }
        else
        {
            if (flag & 0x10)
                dx = 0;
            else
            {
                dx = m_reader->ReadInt16(p);
                p += 2;
            }
        }

        currentX += dx;
        outGlyph.points[i].x = currentX;
    }

    int16_t currentY = 0;

    for (uint16_t i = 0; i < numPoints; i++)
    {
        uint8_t flag = outGlyph.flags[i];
        int16_t dy = 0;

        if (flag & 0x04)
        {
            uint8_t value = m_reader->ReadUInt8(p);
            p++;

            dy = (flag & 0x20) ? value : -value;
        }
        else
        {
            if (flag & 0x20)
                dy = 0;
            else
            {
                dy = m_reader->ReadInt16(p);
                p += 2;
            }
        }

        currentY += dy;
        outGlyph.points[i].y = currentY;
    }

    for (uint16_t i = 0; i < numPoints; i++)
    {
        outGlyph.points[i].onCurve =
            (outGlyph.flags[i] & 0x01) != 0;
    }
}
void TTFParser::BuildContours(Glyph& outGlyph) const
{
    outGlyph.contours.clear();

    uint16_t start = 0;

    for (uint16_t end : outGlyph.endPtsOfContours)
    {
        GlyphContour contour;

        for (uint16_t i = start; i <= end; i++)
        {
            contour.points.push_back(outGlyph.points[i]);
        }

        outGlyph.contours.push_back(contour);

        start = end + 1;
    }
}
void TTFParser::BuildSegments(Glyph& outGlyph) const
{
    outGlyph.segmentContours.clear();

    for (const GlyphContour& contour : outGlyph.contours)
    {
        const auto& sourcePoints = contour.points;

        if (sourcePoints.empty())
            continue;

        /*
            オフカーブ点が連続していた場合、その中点を
            仮想的なオンカーブ点として挿入する。
        */
        std::vector<GlyphPoint> expandedPoints;

        const size_t count = sourcePoints.size();

        for (size_t i = 0; i < count; i++)
        {
            const GlyphPoint& current = sourcePoints[i];
            const GlyphPoint& next =
                sourcePoints[(i + 1) % count];

            expandedPoints.push_back(current);

            if (!current.onCurve && !next.onCurve)
            {
                GlyphPoint middle;

                middle.x = static_cast<int16_t>(
                    (int32_t(current.x) + int32_t(next.x)) / 2
                    );

                middle.y = static_cast<int16_t>(
                    (int32_t(current.y) + int32_t(next.y)) / 2
                    );

                middle.onCurve = true;

                expandedPoints.push_back(middle);
            }
        }

        /*
            輪郭の開始地点をオンカーブ点にそろえる。
            expandedPointsには必ずオンカーブ点が存在する。
        */
        size_t firstOnCurve = 0;

        while (firstOnCurve < expandedPoints.size() &&
            !expandedPoints[firstOnCurve].onCurve)
        {
            firstOnCurve++;
        }

        if (firstOnCurve == expandedPoints.size())
            continue;

        std::rotate(
            expandedPoints.begin(),
            expandedPoints.begin() + firstOnCurve,
            expandedPoints.end()
        );

        // 輪郭を閉じるため、先頭点を末尾にも追加する
        expandedPoints.push_back(expandedPoints.front());

        GlyphSegmentContour segmentContour;

        size_t i = 0;

        while (i + 1 < expandedPoints.size())
        {
            const GlyphPoint& startPoint = expandedPoints[i];
            const GlyphPoint& nextPoint = expandedPoints[i + 1];

            // 現在地点は必ずオンカーブ点
            if (!startPoint.onCurve)
                return;

            if (nextPoint.onCurve)
            {
                GlyphSegment segment;

                segment.type = GlyphSegmentType::Line;
                segment.start = ToVec2(startPoint);
                segment.end = ToVec2(nextPoint);

                segmentContour.segments.push_back(segment);

                i += 1;
            }
            else
            {
                if (i + 2 >= expandedPoints.size())
                    break;

                const GlyphPoint& endPoint =
                    expandedPoints[i + 2];

                if (!endPoint.onCurve)
                    return;

                GlyphSegment segment;

                segment.type = GlyphSegmentType::Quadratic;
                segment.start = ToVec2(startPoint);
                segment.control = ToVec2(nextPoint);
                segment.end = ToVec2(endPoint);

                segmentContour.segments.push_back(segment);

                i += 2;
            }
        }

        outGlyph.segmentContours.push_back(
            std::move(segmentContour)
        );
    }
}

void TTFParser::FlattenSegments(Glyph& outGlyph) const
{
    outGlyph.outline.contours.clear();

    constexpr int CURVE_STEPS = 8;

    for (const GlyphSegmentContour& contour :
        outGlyph.segmentContours)
    {
        std::vector<Vec2> linePoints;

        if (contour.segments.empty())
            continue;

        linePoints.push_back(
            contour.segments.front().start
        );

        for (const GlyphSegment& segment :
            contour.segments)
        {
            if (segment.type == GlyphSegmentType::Line)
            {
                linePoints.push_back(segment.end);
            }
            else
            {
                for (int step = 1;
                    step <= CURVE_STEPS;
                    step++)
                {
                    float t =
                        static_cast<float>(step) /
                        static_cast<float>(CURVE_STEPS);

                    linePoints.push_back(
                        QuadraticBezier(
                            segment.start,
                            segment.control,
                            segment.end,
                            t
                        )
                    );
                }
            }
        }

        outGlyph.outline.contours.push_back(
            std::move(linePoints)
        );
    }
}

bool TTFParser::ReadCmapTable()
{
    const TableRecord* table = FindTable(Tag::CMAP);

    if (!table)
        return false;

    const size_t cmapBase = table->offset;

    uint16_t version = m_reader->ReadUInt16(cmapBase + 0);
    uint16_t numTables = m_reader->ReadUInt16(cmapBase + 2);

    if (version != 0)
        return false;

    uint32_t selectedOffset = 0;

    for (uint16_t i = 0; i < numTables; i++)
    {
        size_t recordOffset = cmapBase + 4 + i * 8;

        uint16_t platformID =
            m_reader->ReadUInt16(recordOffset + 0);

        uint16_t encodingID =
            m_reader->ReadUInt16(recordOffset + 2);

        uint32_t subtableOffset =
            m_reader->ReadUInt32(recordOffset + 4);

        // Windows Unicode BMP
        if (platformID == 3 && encodingID == 1)
        {
            selectedOffset = subtableOffset;
            break;
        }
    }

    if (selectedOffset == 0)
        return false;

    size_t absoluteOffset = cmapBase + selectedOffset;

    uint16_t format = m_reader->ReadUInt16(absoluteOffset);

    if (format != 4)
        return false;

    m_tablesData.cmap.format = format;
    m_tablesData.cmap.subtableOffset =
        static_cast<uint32_t>(absoluteOffset);

    return true;
}
const HorizontalMetric*
TTFParser::GetHorizontalMetric(uint16_t glyphIndex) const
{
    if (glyphIndex >=
        m_tablesData.hmtx.metrics.size())
    {
        return nullptr;
    }

    return &m_tablesData.hmtx.metrics[glyphIndex];
}
uint16_t TTFParser::GetGlyphIndexFormat4(uint32_t codePoint) const
{
    // Format 4はBMP専用
    if (codePoint > 0xFFFF)
        return 0;

    const size_t base =
        m_tablesData.cmap.subtableOffset;

    uint16_t segCountX2 =
        m_reader->ReadUInt16(base + 6);

    uint16_t segCount = segCountX2 / 2;

    size_t endCodeOffset = base + 14;

    size_t startCodeOffset =
        endCodeOffset + segCount * 2 + 2;

    size_t idDeltaOffset =
        startCodeOffset + segCount * 2;

    size_t idRangeOffsetOffset =
        idDeltaOffset + segCount * 2;

    uint16_t characterCode =
        static_cast<uint16_t>(codePoint);

    for (uint16_t i = 0; i < segCount; i++)
    {
        uint16_t endCode =
            m_reader->ReadUInt16(endCodeOffset + i * 2);

        if (characterCode > endCode)
            continue;

        uint16_t startCode =
            m_reader->ReadUInt16(startCodeOffset + i * 2);

        if (characterCode < startCode)
            return 0;

        int16_t idDelta =
            m_reader->ReadInt16(idDeltaOffset + i * 2);

        uint16_t idRangeOffset =
            m_reader->ReadUInt16(
                idRangeOffsetOffset + i * 2
            );

        if (idRangeOffset == 0)
        {
            return static_cast<uint16_t>(
                characterCode + idDelta
                );
        }

        /*
            idRangeOffsetは、自分自身の格納位置を基準にした
            glyphIdArrayへの相対オフセットです。
        */
        size_t currentRangeOffsetAddress =
            idRangeOffsetOffset + i * 2;

        size_t glyphAddress =
            currentRangeOffsetAddress +
            idRangeOffset +
            (characterCode - startCode) * 2;

        uint16_t glyphIndex =
            m_reader->ReadUInt16(glyphAddress);

        if (glyphIndex == 0)
            return 0;

        return static_cast<uint16_t>(
            glyphIndex + idDelta
            );
    }

    return 0;
}
bool TTFParser::ReadHheaTable()
{
    const TableRecord* table = FindTable(Tag::HHEA);

    if (!table)
        return false;

    const size_t o = table->offset;

    m_tablesData.hhea.ascent =
        m_reader->ReadInt16(o + 4);

    m_tablesData.hhea.descent =
        m_reader->ReadInt16(o + 6);

    m_tablesData.hhea.lineGap =
        m_reader->ReadInt16(o + 8);

    m_tablesData.hhea.numberOfHMetrics =
        m_reader->ReadUInt16(o + 34);

    return true;
}
bool TTFParser::ReadHmtxTable()
{
    const TableRecord* table = FindTable(Tag::HMTX);

    if (!table)
        return false;

    const uint16_t numGlyphs =
        m_tablesData.maxp.numGlyphs;

    const uint16_t numberOfHMetrics =
        m_tablesData.hhea.numberOfHMetrics;

    if (numberOfHMetrics == 0 ||
        numberOfHMetrics > numGlyphs)
    {
        return false;
    }

    m_tablesData.hmtx.metrics.resize(numGlyphs);

    size_t p = table->offset;

    uint16_t lastAdvanceWidth = 0;

    for (uint16_t i = 0;
        i < numberOfHMetrics;
        i++)
    {
        HorizontalMetric metric;

        metric.advanceWidth =
            m_reader->ReadUInt16(p);

        metric.leftSideBearing =
            m_reader->ReadInt16(p + 2);

        p += 4;

        m_tablesData.hmtx.metrics[i] = metric;
        lastAdvanceWidth = metric.advanceWidth;
    }

    for (uint16_t i = numberOfHMetrics;
        i < numGlyphs;
        i++)
    {
        HorizontalMetric metric;

        metric.advanceWidth = lastAdvanceWidth;
        metric.leftSideBearing =
            m_reader->ReadInt16(p);

        p += 2;

        m_tablesData.hmtx.metrics[i] = metric;
    }

    return true;
}
Vec2 TTFParser::ToVec2(const GlyphPoint& p)
{
    return Vec2{float(p.x),float(p.y)};
}
Vec2 TTFParser::MidPoint(const Vec2& a, const Vec2& b)
{
    return Vec2{ (a.x + b.x) * 0.5f,(a.y + b.y) * 0.5f };
}
Vec2 TTFParser::QuadraticBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, float t)
{
    float u = 1.0f - t;
    return Vec2{
        u*u*p0.x+2.0f*u*t*p1.x+t*t*p2.x,
        u*u*p0.y+2.0f*u*t*p1.y+t*t*p2.y
    };
}
uint32_t TTFParser::ReadTag(size_t offset) const
{
    return m_reader->ReadUInt32(offset);
}


