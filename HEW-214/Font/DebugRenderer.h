#pragma once

#include "Glyph.h"
#include "DebugImage.h"

class DebugRenderer
{
public:
	static void DrawGlyphToDebugImage(
		DebugImage& image,
		const Glyph& glyph,
		float originX,
		float originY,
		float scale);
};
