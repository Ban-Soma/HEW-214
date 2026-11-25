#pragma once
#include "DirectX.h"
#include <d3d11.h>

// C++14 / DirectX11. Call BeginScene before world draw, EndScene before HUD draw.
class BloomEffect
{
public:
    BloomEffect() = default;
    ~BloomEffect() { Shutdown(); }
    BloomEffect(const BloomEffect&) = delete;
    BloomEffect& operator=(const BloomEffect&) = delete;

    bool Initialize(UINT width, UINT height);
    bool Resize(UINT width, UINT height);
    void Shutdown();
    bool BeginScene(ID3D11DepthStencilView* sceneDepth);
    void EndScene(ID3D11RenderTargetView* backBuffer);

    void SetThreshold(float threshold) { m_threshold = threshold; }
    void SetStrength(float strength) { m_strength = strength; }

private:
    struct Target
    {
        ID3D11Texture2D* texture = nullptr;
        ID3D11RenderTargetView* rtv = nullptr;
        ID3D11ShaderResourceView* srv = nullptr;
    };
    struct alignas(16) Parameters
    {
        float threshold;
        float strength;
        float invWidth;
        float invHeight;
    };

    bool CreateTarget(Target& t, UINT width, UINT height);
    bool CreateShaders();
    void ReleaseTargets();
    void ReleaseShaders();
    void DrawPass(ID3D11PixelShader* shader, ID3D11RenderTargetView* dst,
        ID3D11ShaderResourceView* input0, ID3D11ShaderResourceView* input1,
        UINT width, UINT height, const Parameters& parameters);

    UINT m_width = 0;
    UINT m_height = 0;
    UINT m_halfWidth = 0;
    UINT m_halfHeight = 0;
    float m_threshold = 0.85f;
    float m_strength = 0.85f;
    bool m_begun = false;
    ID3D11BlendState* m_savedBlend = nullptr;
    ID3D11DepthStencilState* m_savedDepth = nullptr;
    ID3D11RasterizerState* m_savedRaster = nullptr;
    ID3D11DepthStencilView* m_savedDSV = nullptr;
    FLOAT m_savedBlendFactor[4] = {};
    UINT m_savedSampleMask = 0xffffffff;
    UINT m_savedStencilRef = 0;

    Target m_scene;
    Target m_ping;
    Target m_pong;
    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_brightPS = nullptr;
    ID3D11PixelShader* m_blurHPS = nullptr;
    ID3D11PixelShader* m_blurVPS = nullptr;
    ID3D11PixelShader* m_compositePS = nullptr;
    ID3D11Buffer* m_params = nullptr;
    ID3D11SamplerState* m_sampler = nullptr;
    ID3D11DepthStencilState* m_noDepth = nullptr;
    ID3D11BlendState* m_opaque = nullptr;
    ID3D11RasterizerState* m_noCull = nullptr;
};
