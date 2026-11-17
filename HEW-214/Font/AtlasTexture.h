#pragma once

#include <d3d11.h>

class TextureAtlas;

struct AtlasTextureDX11
{
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    int width = 0;
    int height = 0;

    bool IsValid() const { return texture != nullptr && srv != nullptr; }
};

bool CreateAtlasTexture(ID3D11Device* device, const TextureAtlas& atlas, AtlasTextureDX11& outTexture);
bool UpdateAtlasTexture(ID3D11DeviceContext* context, AtlasTextureDX11& texture, const TextureAtlas& atlas);
void DeleteAtlasTexture(AtlasTextureDX11& texture);
