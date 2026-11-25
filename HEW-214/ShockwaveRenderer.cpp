
#include "ShockwaveRenderer.h"
#include <algorithm>

using namespace DirectX;

namespace
{
    struct WaveVertex
    {
        XMFLOAT3 pos;
        XMFLOAT2 uv;
    };

    struct WaveParams
    {

            DirectX::XMFLOAT4 color;
            DirectX::XMFLOAT4 settings;
            DirectX::XMFLOAT4 effect;        
    };

    const char* WaveVS = R"(
        cbuffer Matrix : register(b0)
        {
            float4x4 world;
            float4x4 view;
            float4x4 proj;
        };

        struct VS_IN
        {
            float3 pos : POSITION0;
            float2 uv  : TEXCOORD0;
        };

        struct VS_OUT
        {
            float4 pos : SV_POSITION;
            float2 uv  : TEXCOORD0;
        };

        VS_OUT main(VS_IN input)
        {
            VS_OUT output;

            float4 p = float4(input.pos, 1.0f);

            p = mul(p, world);
            p = mul(p, view);
            p = mul(p, proj);

            output.pos = p;
            output.uv = input.uv;

            return output;
        }
    )";

    const char* WavePS = R"(
        
cbuffer WaveBuffer : register(b0)
{
    float4 waveColor;
    float4 settings;
    // x = progress
    // y = ringWidth
    // z = glowWidth
    // w = opacity

    float4 effect;
    // x = mode (0: electric ring, 1: floor glow)
    // y = time
    // z,w = unused
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// 擬似乱数
float Hash(float n)
{
    return frac(sin(n * 127.1) * 43758.5453);
}

// 連続したノイズ
float Noise(float n)
{
    float i = floor(n);
    float f = frac(n);

    f = f * f * (3.0 - 2.0 * f);

    return lerp(Hash(i), Hash(i + 1.0), f);
}

float4 main(PS_IN input) : SV_TARGET0
{
    float2 uv = input.uv * 2.0 - 1.0;
    float dist = length(uv);

    float progress = saturate(settings.x);
    float opacity = settings.w;
    float fade = 1.0 - progress;

    // 床の発光用
    if (effect.x > 0.5)
    {
        float light = exp(-dist * dist * 4.0);

        // 中心付近を強く発光
        light *= 0.55 + 0.45 * (1.0 - progress);

        float alpha =
            light * opacity * fade * waveColor.a;
        
        float3 emission =waveColor.rgb*3.0f;

        return float4(emission, saturate(alpha));
    }

    // ------------------------
    // 電撃リング
    // ------------------------

    float angle = atan2(uv.y, uv.x);

    // 時間変化する輪郭の歪み
    float noise1 = Noise(angle * 23.0 + effect.y * 17.0);
    float noise2 = Noise(angle * 61.0 - effect.y * 29.0);

    float distortion =
        (noise1 - 0.5) * 0.075 +
        (noise2 - 0.5) * 0.035;

    // リングが外側へ広がる
    float radius = 0.08 + progress * 0.82;

    radius += distortion;

    float d = abs(dist - radius);

    // 明るい芯
    float core =
        1.0 - smoothstep(0.0, settings.y, d);

    // 周辺の青い光
    float glow =
        1.0 - smoothstep(0.0, settings.z, d);

    // 部分的な明滅
    float flicker =
        0.65 + 0.35 *
        Noise(angle * 47.0 + effect.y * 38.0);

    // 稲妻の枝のような小さな突出
    float branchNoise =
        Noise(angle * 38.0 + effect.y * 12.0);

    float branch =
        smoothstep(0.82, 0.98, branchNoise);

    float branchDist =
        abs(dist - radius - branch * 0.07);

    float sparks =
        (1.0 - smoothstep(0.0, 0.02, branchDist))
        * branch * 0.55;

    float edgeFade =
        1.0 - smoothstep(0.88, 1.0, dist);

    float intensity =
        (core + glow * 0.35 + sparks)
        * flicker
        * edgeFade
        * fade;

    float alpha =
        saturate(intensity * opacity * waveColor.a);

    return float4(waveColor.rgb, alpha);
}

    )";
}

ShockwaveRenderer::ShockwaveRenderer()
{
}

ShockwaveRenderer::~ShockwaveRenderer()
{
    Uninit();
}

bool ShockwaveRenderer::Init()
{
    m_pVS = new VertexShader();
    m_pPS = new PixelShader();

    if (FAILED(m_pVS->Compile(WaveVS)) ||
        FAILED(m_pPS->Compile(WavePS)))
    {
        Uninit();
        return false;
    }

    const WaveVertex vertices[6] =
    {
        {{-1, 0, -1}, {0, 0}},
        {{ 1, 0, -1}, {1, 0}},
        {{-1, 0,  1}, {0, 1}},

        {{-1, 0,  1}, {0, 1}},
        {{ 1, 0, -1}, {1, 0}},
        {{ 1, 0,  1}, {1, 1}}
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;

    HRESULT hr = GetDevice()->CreateBuffer(
        &vbDesc, &vbData, &m_pVertexBuffer);

    if (FAILED(hr))
    {
        Uninit();
        return false;
    }


    D3D11_BLEND_DESC blendDesc = {};
    auto& rt = blendDesc.RenderTarget[0];

    rt.BlendEnable = TRUE;
    rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rt.DestBlend = D3D11_BLEND_ONE;
    rt.BlendOp = D3D11_BLEND_OP_ADD;

    rt.SrcBlendAlpha = D3D11_BLEND_ONE;
    rt.DestBlendAlpha = D3D11_BLEND_ONE;
    rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rt.RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = GetDevice()->CreateBlendState(
        &blendDesc, &m_pBlendState);

    if (FAILED(hr))
    {
        Uninit();
        return false;
    }

    
    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    hr = GetDevice()->CreateDepthStencilState(
        &depthDesc, &m_pDepthState);

    if (FAILED(hr))
    {
        Uninit();
        return false;
    }


    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable = TRUE;

    hr = GetDevice()->CreateRasterizerState(
        &rasterDesc, &m_pRasterState);

    if (FAILED(hr))
    {
        Uninit();
        return false;
    }

    return true;
}

void ShockwaveRenderer::Uninit()
{
    SAFE_RELEASE(m_pRasterState);
    SAFE_RELEASE(m_pDepthState);
    SAFE_RELEASE(m_pBlendState);
    SAFE_RELEASE(m_pVertexBuffer);

    SAFE_DELETE(m_pVS);
    SAFE_DELETE(m_pPS);
}

void ShockwaveRenderer::Draw(
    const XMFLOAT3& pos,
    float maxRadius,
    float progress,
    const XMFLOAT4X4& view,
    const XMFLOAT4X4& projection,
    bool floorGlow)
{
    if (!m_pVS || !m_pPS || !m_pVertexBuffer)
        return;

    auto* context = GetContext();

    // ワールド行列
    XMMATRIX world =
        XMMatrixScaling(maxRadius, 1.0f, maxRadius) *
        XMMatrixTranslation(
            pos.x,
            0.5f,
            pos.z
        );

    // 既存のGeometoryと同じ行列の渡し方
    XMFLOAT4X4 matrices[3];

    XMStoreFloat4x4(
        &matrices[0],
        XMMatrixTranspose(world)
    );

    // Camera側の行列は既存の
    // ShaderListと同じ形式を使用
    matrices[1] = view;
    matrices[2] = projection;

    m_pVS->WriteBuffer(0, matrices);

    // 衝撃波のパラメータ
    WaveParams params = {};

    params.color = DirectX::XMFLOAT4(
        0.25f, 0.85f, 1.0f, 1.0f
    );

    params.settings = DirectX::XMFLOAT4(
       ( std::max)(0.0f, (std::min)(progress, 1.0f)),
        0.025f,  // 電撃の芯の太さ
        0.12f,   // 周囲の発光
        floorGlow ? 0.45f : 1.0f
    );

    params.effect = DirectX::XMFLOAT4(
        floorGlow ? 1.0f : 0.0f,
        progress * 2.0f,
        0.0f,
        0.0f
    );

    m_pPS->WriteBuffer(0, &params);

    // 既存の描画ステートを退避
    ID3D11BlendState* oldBlend = nullptr;
    ID3D11DepthStencilState* oldDepth = nullptr;
    ID3D11RasterizerState* oldRaster = nullptr;

    FLOAT oldBlendFactor[4] = {};
    UINT oldSampleMask = 0;
    UINT oldStencilRef = 0;

    context->OMGetBlendState(
        &oldBlend, oldBlendFactor, &oldSampleMask
    );

    context->OMGetDepthStencilState(
        &oldDepth, &oldStencilRef
    );

    context->RSGetState(&oldRaster);

    // 衝撃波用の描画ステート
    const FLOAT blendFactor[4] = { 0, 0, 0, 0 };

    context->OMSetBlendState(
        m_pBlendState, blendFactor, 0xffffffff
    );
    context->OMSetDepthStencilState(
        m_pDepthState, 0
    );
    context->RSSetState(m_pRasterState);

    // シェーダー設定
    m_pVS->Bind();
    m_pPS->Bind();

    UINT stride = sizeof(WaveVertex);
    UINT offset = 0;

    context->IASetVertexBuffers(
        0, 1, &m_pVertexBuffer, &stride, &offset
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->Draw(6, 0);

    // 描画ステートを復元
    context->OMSetBlendState(
        oldBlend, oldBlendFactor, oldSampleMask
    );
    context->OMSetDepthStencilState(
        oldDepth, oldStencilRef
    );
    context->RSSetState(oldRaster);

    SAFE_RELEASE(oldBlend);
    SAFE_RELEASE(oldDepth);
    SAFE_RELEASE(oldRaster);
}
