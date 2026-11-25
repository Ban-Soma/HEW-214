#pragma once

#include "Shader.h"
#include <DirectXMath.h>

class ShockwaveRenderer
{
public:
    ShockwaveRenderer();
    ~ShockwaveRenderer();

    bool Init();
    void Uninit();

    void Draw(const DirectX::XMFLOAT3& pos,float maxRadius,float progress,const DirectX::XMFLOAT4X4& view,const DirectX::XMFLOAT4X4& projection,bool floorGlow= false);

private:
    VertexShader* m_pVS = nullptr;
    PixelShader* m_pPS = nullptr;

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11BlendState* m_pBlendState = nullptr;
    ID3D11DepthStencilState* m_pDepthState = nullptr;
    ID3D11RasterizerState* m_pRasterState = nullptr;
};
