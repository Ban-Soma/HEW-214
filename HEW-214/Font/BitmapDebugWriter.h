#pragma once

#include "GlyphBitmap.h"
#include "TextBitmap.h"
#include <string>

#include "TextureAtlas.h"


bool SaveGlyphBitmapPGM(
    const GlyphBitmap& bitmap,
    const std::string& filename);

bool SaveTextBitmapPGM(
    const TextBitmap& bitmap,
    const std::string& filename);

bool SaveTextureAtlasPGM(
    const TextureAtlas& atlas,
    const std::string& filename
);