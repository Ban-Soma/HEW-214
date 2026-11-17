#include "SpriteFontRenderer.h"

#include <d3dcompiler.h>
#include <vector>
#include <cstdint>
#include <cstring>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace
{
    struct FontVertexDX11
    {
        float x, y;
        float u, v;
        float r, g, b, a;
    };

    ID3D11VertexShader* g_vs = nullptr;
    ID3D11PixelShader* g_ps = nullptr;
    ID3D11InputLayout* g_layout = nullptr;
    ID3D11Buffer* g_vertexBuffer = nullptr;
    UINT g_vertexCapacity = 0;
    ID3D11SamplerState* g_sampler = nullptr;
    ID3D11BlendState* g_blend = nullptr;
    ID3D11RasterizerState* g_rasterizer = nullptr;
    ID3D11DepthStencilState* g_depthState = nullptr;
    ID3D11Device* g_device = nullptr;

    const char* kShaderSource = R"(
struct VSIn
{
    float2 pos   : POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

struct PSIn
{
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

PSIn VSMain(VSIn input)
{
    PSIn o;
    o.pos = float4(input.pos, 0.0f, 1.0f);
    o.uv = input.uv;
    o.color = input.color;
    return o;
}

Texture2D FontTexture : register(t0);
SamplerState FontSampler : register(s0);

float4 PSMain(PSIn input) : SV_TARGET
{
    return FontTexture.Sample(FontSampler, input.uv) * input.color;
}
)";

    template<class T>
    void SafeRelease(T*& p)
    {
        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }

    bool EnsureVertexBuffer(UINT requiredVertices)
    {
        if (!g_device)
            return false;

        if (g_vertexBuffer && g_vertexCapacity >= requiredVertices)
            return true;

        SafeRelease(g_vertexBuffer);
        g_vertexCapacity = 0;

        UINT capacity = 256;
        while (capacity < requiredVertices)
            capacity *= 2;

        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = capacity * static_cast<UINT>(sizeof(FontVertexDX11));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(g_device->CreateBuffer(&desc, nullptr, &g_vertexBuffer)))
            return false;

        g_vertexCapacity = capacity;
        return true;
    }
}

bool InitializeSpriteFontRendererDX11(ID3D11Device* device)
{
    FinalizeSpriteFontRendererDX11();
    if (!device)
        return false;

    g_device = device;
    g_device->AddRef();

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompile(
        kShaderSource, std::strlen(kShaderSource), nullptr, nullptr, nullptr,
        "VSMain", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &vsBlob, &errorBlob);
    SafeRelease(errorBlob);
    if (FAILED(hr))
    {
        SafeRelease(vsBlob);
        FinalizeSpriteFontRendererDX11();
        return false;
    }

    hr = D3DCompile(
        kShaderSource, std::strlen(kShaderSource), nullptr, nullptr, nullptr,
        "PSMain", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &psBlob, &errorBlob);
    SafeRelease(errorBlob);
    if (FAILED(hr))
    {
        SafeRelease(vsBlob);
        SafeRelease(psBlob);
        FinalizeSpriteFontRendererDX11();
        return false;
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_vs);
    if (FAILED(hr)) goto fail;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_ps);
    if (FAILED(hr)) goto fail;

    {
        D3D11_INPUT_ELEMENT_DESC elems[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = device->CreateInputLayout(
            elems, 3,
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            &g_layout);
        if (FAILED(hr)) goto fail;
    }

    {
        D3D11_SAMPLER_DESC desc{};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&desc, &g_sampler);
        if (FAILED(hr)) goto fail;
    }

    {
        D3D11_BLEND_DESC desc{};
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&desc, &g_blend);
        if (FAILED(hr)) goto fail;
    }

    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.DepthClipEnable = TRUE;
        hr = device->CreateRasterizerState(&desc, &g_rasterizer);
        if (FAILED(hr)) goto fail;
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        hr = device->CreateDepthStencilState(&desc, &g_depthState);
        if (FAILED(hr)) goto fail;
    }

    SafeRelease(vsBlob);
    SafeRelease(psBlob);
    return true;

fail:
    SafeRelease(vsBlob);
    SafeRelease(psBlob);
    FinalizeSpriteFontRendererDX11();
    return false;
}

void FinalizeSpriteFontRendererDX11()
{
    SafeRelease(g_vertexBuffer);
    SafeRelease(g_layout);
    SafeRelease(g_vs);
    SafeRelease(g_ps);
    SafeRelease(g_sampler);
    SafeRelease(g_blend);
    SafeRelease(g_rasterizer);
    SafeRelease(g_depthState);
    SafeRelease(g_device);
    g_vertexCapacity = 0;
}

bool DrawTextMeshWithSpriteDX11(
    ID3D11DeviceContext* context,
    const TextMesh& mesh,
    ID3D11ShaderResourceView* textureSRV,
    float offsetX,
    float offsetY,
    float r,
    float g,
    float b,
    float a)
{
    if (!context || !textureSRV || !mesh.IsValid() || !g_device)
        return false;

    UINT viewportCount = 1;
    D3D11_VIEWPORT viewport{};
    context->RSGetViewports(&viewportCount, &viewport);
    if (viewportCount == 0 || viewport.Width <= 0.0f || viewport.Height <= 0.0f)
        return false;

    std::vector<FontVertexDX11> vertices;
    vertices.reserve(mesh.indices.size());

    for (uint32_t index : mesh.indices)
    {
        if (index >= mesh.vertices.size())
            return false;

        const TextVertex& v = mesh.vertices[index];
        const float px = v.x + offsetX;
        const float py = v.y + offsetY;

        // Pixel coordinates (top-left origin) -> DirectX NDC.
        const float ndcX = (px / viewport.Width) * 2.0f - 1.0f;
        const float ndcY = 1.0f - (py / viewport.Height) * 2.0f;

        vertices.push_back({ ndcX, ndcY, v.u, v.v, r, g, b, a });
    }

    if (!EnsureVertexBuffer(static_cast<UINT>(vertices.size())))
        return false;

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(context->Map(g_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;

    std::memcpy(mapped.pData, vertices.data(), vertices.size() * sizeof(FontVertexDX11));
    context->Unmap(g_vertexBuffer, 0);

    UINT stride = sizeof(FontVertexDX11);
    UINT offset = 0;
    context->IASetInputLayout(g_layout);
    context->IASetVertexBuffers(0, 1, &g_vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(g_vs, nullptr, 0);
    context->PSSetShader(g_ps, nullptr, 0);
    context->PSSetShaderResources(0, 1, &textureSRV);
    context->PSSetSamplers(0, 1, &g_sampler);

    const float blendFactor[4] = { 0, 0, 0, 0 };
    context->OMSetBlendState(g_blend, blendFactor, 0xffffffff);
    context->OMSetDepthStencilState(g_depthState, 0);
    context->RSSetState(g_rasterizer);

    context->Draw(static_cast<UINT>(vertices.size()), 0);

    // Avoid keeping the atlas SRV bound if caller wants to update/recreate it later.
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);
    return true;
}
