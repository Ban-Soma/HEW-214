#pragma once

#include <d3d11.h>
#include "TextMesh.h"

bool InitializeSpriteFontRendererDX11(ID3D11Device* device);
void FinalizeSpriteFontRendererDX11();

bool DrawTextMeshWithSpriteDX11(
    ID3D11DeviceContext* context,
    const TextMesh& mesh,
    ID3D11ShaderResourceView* textureSRV,
    float offsetX,
    float offsetY,
    float r,
    float g,
    float b,
    float a);
