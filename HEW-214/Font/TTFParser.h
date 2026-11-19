#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "BinaryReader.h"
#include "Tables.h"
#include "Glyph.h"

struct TableRecord
{
	uint32_t tag;
	uint32_t checkSum;
	uint32_t offset;
	uint32_t length;
};



class TTFParser
{
public:
	bool Load(const std::string& filename);

	const TableRecord* FindTable(uint32_t tag)const;
	bool ReadGlyph(uint16_t glyphIndex, Glyph& outGlyph)const;

	uint16_t GetGlyphIndex(uint32_t codePoint)const;
private:
	bool ReadOffsetTable();
	bool ReadTableDirectory();
	bool ReadHeadTable();
	bool ReadMaxpTable();
	bool ReadLocaTable();
	
	bool ReadGlyphInternal(uint16_t glyphIndex, Glyph& outGlyph, int depth) const;
	bool ReadGlyphBasic(uint16_t glyphIndex, Glyph& outGlyph, size_t& outGlyphDataOffset)const;
	bool ReadSimpleGlyph(Glyph& outGlyph, size_t glyphDataOffset) const;
	void ReadGlyphFlags(Glyph& outGlyph, size_t& p, uint16_t numPoints)const;
	void ReadGlyphCoordinates(Glyph& outGlyph, size_t& p, uint16_t numPoints)const;
	void BuildContours(Glyph& outGlyph)const;
	//void BuildOutline(Glyph& outGlyph)const;
	void BuildSegments(Glyph& outGlyph) const;
	void FlattenSegments(Glyph& outGlyph) const;
	bool ReadCmapTable();

	bool ReadHheaTable();
	bool ReadHmtxTable();

	uint16_t GetGlyphIndexFormat4(uint32_t codePoint)const;

	static Vec2 ToVec2(const GlyphPoint& p);
	static Vec2 MidPoint(const Vec2& a, const Vec2& b);
	static Vec2 QuadraticBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, float t);

	uint32_t ReadTag(size_t offset)const;

public:
	uint16_t GetUnitsPerEm() const
	{
		return m_tablesData.head.unitsPerEm;
	}
	const HorizontalMetric* GetHorizontalMetric(uint16_t glyphIndex) const;

	int16_t GetAscent() const
	{
		return m_tablesData.hhea.ascent;
	}

	int16_t GetDescent() const
	{
		return m_tablesData.hhea.descent;
	}

	int16_t GetLineGap() const
	{
		return m_tablesData.hhea.lineGap;
	}
private:
	std::vector<uint8_t> m_data;
	size_t m_sfntOffset = 0;
	uint16_t m_numTables = 0;
	std::unordered_map<uint32_t, TableRecord> m_tables;
	//BinaryReader* m_reader = nullptr;
	std::unique_ptr<BinaryReader> m_reader;
	FontTables m_tablesData;
};