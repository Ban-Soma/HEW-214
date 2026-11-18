#pragma once

class TextureAtlas;
struct TextMesh;

class IFontRenderer
{
public:
    virtual ~IFontRenderer() = default;

    virtual void DrawTextMesh(
        const TextMesh& mesh,
        const TextureAtlas& atlas
    ) = 0;
};
