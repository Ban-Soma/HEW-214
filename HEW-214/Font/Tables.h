#pragma once
#include <cstdint>
#include <vector>

struct HeadTable
{
	uint16_t unitsPerEm;

	int16_t xMin;
	int16_t yMin;
	int16_t xMax;
	int16_t yMax;

	int16_t indexToLocFormat;
};

struct MaxpTable
{
	uint32_t version;
	uint16_t numGlyphs;
};


struct LocaTable
{
	std::vector<uint32_t> offsets;
};

struct CmapTable
{
	uint16_t format = 0;
	uint32_t subtableOffset = 0;
};
struct HheaTable
{
	int16_t ascent = 0;
	int16_t descent = 0;
	int16_t lineGap = 0;

	uint16_t numberOfHMetrics = 0;
};

struct HorizontalMetric
{
	uint16_t advanceWidth = 0;
	int16_t leftSideBearing = 0;
};

struct HmtxTable
{
	std::vector<HorizontalMetric> metrics;
};

struct FontTables
{
	HeadTable head;

	MaxpTable maxp;

	LocaTable loca;

	CmapTable cmap;

	HheaTable hhea;
	HmtxTable hmtx;
};


