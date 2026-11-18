#pragma once

enum class TextAlignment
{
	Left,
	Center,
	Right
};
struct TextLayout
{
	float maxWidth = 0.0f;
	float lineSpacing = 0.0f;
	TextAlignment alignment = TextAlignment::Left;
};