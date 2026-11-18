#include "DebugRenderer.h"

void DebugRenderer::DrawGlyphToDebugImage(DebugImage& image, const Glyph& glyph, float originX, float originY, float scale)
{
	for (const auto& contour : glyph.outline.contours)
	{
		if (contour.size() < 2)continue;

		for (size_t i = 0;i < contour.size();i++)
		{
			const Vec2& a = contour[i];
			const Vec2& b = contour[(i + 1) % contour.size()];

			int ax = static_cast<int>(originX + a.x * scale);
			int ay = static_cast<int>(originY - a.y * scale);

			int bx = static_cast<int>(originX + b.x * scale);
			int by = static_cast<int>(originY - b.y * scale);

			image.DrawLine(ax, ay, bx, by);
		}
	}
}
