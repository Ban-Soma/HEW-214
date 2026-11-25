#include "BloomEffect.h"
#include <d3dcompiler.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#pragma comment(lib, "d3dcompiler.lib")

namespace
{
    template<class T> void Release(T*& p) { if (p) { p->Release(); p = nullptr; } }

    const char* kVS = R"(
        struct Output { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
        Output main(uint id : SV_VertexID)
        {
            Output o;
            float2 uv = float2((id << 1) & 2, id & 2);
            o.uv = uv;
            o.pos = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, 0.0, 1.0);
            return o;
        }
    )";
    const char* kPSCommon = R"(
        cbuffer Parameters : register(b0)
        {
            float threshold;
            float strength;
            float invWidth;
            float invHeight;
        };
        Texture2D source0 : register(t0);
        Texture2D source1 : register(t1);
        SamplerState linearSampler : register(s0);
        struct Input { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
    )";
    const char* kBright = R"(
        float4 main(Input i) : SV_TARGET
        {
            float3 c = source0.Sample(linearSampler, i.uv).rgb;
            float luma = dot(c, float3(0.2126, 0.7152, 0.0722));
            float factor = saturate((luma - threshold) / 0.25);
            return float4(c * factor, 1.0);
        }
    )";
    const char* kBlurH = R"(
        float4 main(Input i) : SV_TARGET
        {
            float2 stepUV = float2(invWidth, 0);
            float3 c = source0.Sample(linearSampler, i.uv).rgb * 0.227027;
            c += source0.Sample(linearSampler, i.uv + stepUV * 1.384615).rgb * 0.316216;
            c += source0.Sample(linearSampler, i.uv - stepUV * 1.384615).rgb * 0.316216;
            c += source0.Sample(linearSampler, i.uv + stepUV * 3.230769).rgb * 0.070270;
            c += source0.Sample(linearSampler, i.uv - stepUV * 3.230769).rgb * 0.070270;
            return float4(c, 1);
        }
    )";
    const char* kBlurV = R"(
        float4 main(Input i) : SV_TARGET
        {
            float2 stepUV = float2(0, invHeight);
            float3 c = source0.Sample(linearSampler, i.uv).rgb * 0.227027;
            c += source0.Sample(linearSampler, i.uv + stepUV * 1.384615).rgb * 0.316216;
            c += source0.Sample(linearSampler, i.uv - stepUV * 1.384615).rgb * 0.316216;
            c += source0.Sample(linearSampler, i.uv + stepUV * 3.230769).rgb * 0.070270;
            c += source0.Sample(linearSampler, i.uv - stepUV * 3.230769).rgb * 0.070270;
            return float4(c, 1);
        }
    )";
    const char* kComposite = R"(
        float4 main(Input i) : SV_TARGET
        {
            float3 scene = source0.Sample(linearSampler, i.uv).rgb;
            float3 blurred = source1.Sample(linearSampler, i.uv).rgb;
            float3 c = max(scene + blurred * strength, 0.0);
            // Simple output clamp for an SDR back buffer.
            // Replace with exposure + tonemapping when moving to full HDR.
            return float4(saturate(c), 1.0);
        }
    )";

    HRESULT Compile(const char* src, const char* profile, ID3DBlob** blob)
    {
        ID3DBlob* errors = nullptr;
        const HRESULT hr = D3DCompile(src, strlen(src), nullptr, nullptr, nullptr,
            "main", profile, D3DCOMPILE_ENABLE_STRICTNESS, 0, blob, &errors);
        if (errors)
        {
            OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
            errors->Release();
        }
        return hr;
    }
}

bool BloomEffect::CreateTarget(Target& t, UINT w, UINT h)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    ID3D11Device* dev = GetDevice();
    if (FAILED(dev->CreateTexture2D(&desc, nullptr, &t.texture))) return false;
    if (FAILED(dev->CreateRenderTargetView(t.texture, nullptr, &t.rtv))) return false;
    if (FAILED(dev->CreateShaderResourceView(t.texture, nullptr, &t.srv))) return false;
    return true;
}

bool BloomEffect::CreateShaders()
{
    ID3D11Device* dev = GetDevice();
    ID3DBlob* blob = nullptr;
    if (FAILED(Compile(kVS, "vs_5_0", &blob))) return false;
    HRESULT hr = dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_vs);
    Release(blob);
    if (FAILED(hr)) return false;

    const char* snippets[] = { kBright, kBlurH, kBlurV, kComposite };
    ID3D11PixelShader** outputs[] = { &m_brightPS, &m_blurHPS, &m_blurVPS, &m_compositePS };
    for (int i = 0; i < 4; ++i)
    {
        const std::string source = std::string(kPSCommon) + snippets[i];
        if (FAILED(Compile(source.c_str(), "ps_5_0", &blob))) return false;
        hr = dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, outputs[i]);
        Release(blob);
        if (FAILED(hr)) return false;
    }

    D3D11_BUFFER_DESC cb = {};
    cb.ByteWidth = sizeof(Parameters);
    cb.Usage = D3D11_USAGE_DEFAULT;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(dev->CreateBuffer(&cb, nullptr, &m_params))) return false;

    D3D11_SAMPLER_DESC sm = {};
    sm.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sm.AddressU = sm.AddressV = sm.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sm.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(dev->CreateSamplerState(&sm, &m_sampler))) return false;

    D3D11_DEPTH_STENCIL_DESC ds = {};
    ds.DepthEnable = FALSE;
    ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    if (FAILED(dev->CreateDepthStencilState(&ds, &m_noDepth))) return false;

    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(dev->CreateBlendState(&bd, &m_opaque))) return false;

    D3D11_RASTERIZER_DESC rs = {};
    rs.FillMode = D3D11_FILL_SOLID;
    rs.CullMode = D3D11_CULL_NONE;
    rs.DepthClipEnable = TRUE;
    if (FAILED(dev->CreateRasterizerState(&rs, &m_noCull))) return false;
    return true;
}

bool BloomEffect::Initialize(UINT width, UINT height)
{
    Shutdown();
    if (!width || !height) return false;
    m_width = width;
    m_height = height;
    m_halfWidth = (std::max)(1u, (width + 1) / 2);
    m_halfHeight = (std::max)(1u, (height + 1) / 2);
    if (!CreateTarget(m_scene, width, height) ||
        !CreateTarget(m_ping, m_halfWidth, m_halfHeight) ||
        !CreateTarget(m_pong, m_halfWidth, m_halfHeight) ||
        !CreateShaders())
    {
        Shutdown();
        return false;
    }
    return true;
}

bool BloomEffect::Resize(UINT width, UINT height)
{
    return Initialize(width, height);
}

void BloomEffect::ReleaseTargets()
{
    Target* list[] = { &m_scene, &m_ping, &m_pong };
    for (auto* t : list)
    {
        Release(t->srv);
        Release(t->rtv);
        Release(t->texture);
    }
}

void BloomEffect::ReleaseShaders()
{
    Release(m_vs);
    Release(m_brightPS);
    Release(m_blurHPS);
    Release(m_blurVPS);
    Release(m_compositePS);
    Release(m_params);
    Release(m_sampler);
    Release(m_noDepth);
    Release(m_opaque);
    Release(m_noCull);
}

void BloomEffect::Shutdown()
{
    m_begun = false;
    Release(m_savedBlend);
    Release(m_savedDepth);
    Release(m_savedRaster);
    Release(m_savedDSV);
    ReleaseTargets();
    ReleaseShaders();
    m_width = m_height = m_halfWidth = m_halfHeight = 0;
}

bool BloomEffect::BeginScene(ID3D11DepthStencilView* sceneDepth)
{
    if (!m_scene.rtv || !sceneDepth) return false;
    auto* ctx = GetContext();
    // Save game pipeline state so the next frame's 3D draw still has depth testing.
    Release(m_savedBlend);
    Release(m_savedDepth);
    Release(m_savedRaster);
    Release(m_savedDSV);
    ctx->OMGetBlendState(&m_savedBlend, m_savedBlendFactor, &m_savedSampleMask);
    ctx->OMGetDepthStencilState(&m_savedDepth, &m_savedStencilRef);
    ctx->RSGetState(&m_savedRaster);
    ctx->OMGetRenderTargets(0, nullptr, &m_savedDSV);
    ID3D11ShaderResourceView* empty[2] = { nullptr, nullptr };
    ctx->PSSetShaderResources(0, 2, empty);
    ctx->OMSetRenderTargets(1, &m_scene.rtv, sceneDepth);
    D3D11_VIEWPORT vp = { 0, 0, float(m_width), float(m_height), 0, 1 };
    ctx->RSSetViewports(1, &vp);
    const float clearColor[4] = { 0.015f, 0.02f, 0.03f, 1 };
    ctx->ClearRenderTargetView(m_scene.rtv, clearColor);
    ctx->ClearDepthStencilView(sceneDepth, D3D11_CLEAR_DEPTH, 1.0f, 0);
    m_begun = true;
    return true;
}

void BloomEffect::DrawPass(ID3D11PixelShader* shader, ID3D11RenderTargetView* dst,
    ID3D11ShaderResourceView* input0, ID3D11ShaderResourceView* input1,
    UINT width, UINT height, const Parameters& parameters)
{
    auto* ctx = GetContext();
    ID3D11ShaderResourceView* empty[2] = { nullptr, nullptr };
    ctx->PSSetShaderResources(0, 2, empty);
    ctx->OMSetRenderTargets(1, &dst, nullptr);
    D3D11_VIEWPORT vp = { 0, 0, float(width), float(height), 0, 1 };
    ctx->RSSetViewports(1, &vp);
    ctx->UpdateSubresource(m_params, 0, nullptr, &parameters, 0, 0);
    ctx->PSSetConstantBuffers(0, 1, &m_params);
    ctx->VSSetShader(m_vs, nullptr, 0);
    ctx->PSSetShader(shader, nullptr, 0);
    ctx->PSSetSamplers(0, 1, &m_sampler);
    ID3D11ShaderResourceView* resources[2] = { input0, input1 };
    ctx->PSSetShaderResources(0, 2, resources);
    ctx->IASetInputLayout(nullptr);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D11Buffer* nullBuffer = nullptr;
    UINT zero = 0;
    ctx->IASetVertexBuffers(0, 1, &nullBuffer, &zero, &zero);
    ctx->Draw(3, 0);
    ctx->PSSetShaderResources(0, 2, empty);
}

void BloomEffect::EndScene(ID3D11RenderTargetView* backBuffer)
{
    if (!m_begun || !backBuffer) return;
    m_begun = false;
    auto* ctx = GetContext();
    const float blendFactor[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(m_opaque, blendFactor, 0xffffffff);
    ctx->OMSetDepthStencilState(m_noDepth, 0);
    ctx->RSSetState(m_noCull);

    // Threshold at half resolution, then horizontal and vertical blur.
    const Parameters bright = { m_threshold, m_strength, 1.0f / m_halfWidth, 1.0f / m_halfHeight };
    DrawPass(m_brightPS, m_ping.rtv, m_scene.srv, nullptr, m_halfWidth, m_halfHeight, bright);
    DrawPass(m_blurHPS, m_pong.rtv, m_ping.srv, nullptr, m_halfWidth, m_halfHeight, bright);
    DrawPass(m_blurVPS, m_ping.rtv, m_pong.srv, nullptr, m_halfWidth, m_halfHeight, bright);

    DrawPass(m_compositePS, backBuffer, m_scene.srv, m_ping.srv, m_width, m_height, bright);

    // Restore OM/RS state for subsequent HUD and following frames.
    ctx->OMSetBlendState(m_savedBlend, m_savedBlendFactor, m_savedSampleMask);
    ctx->OMSetDepthStencilState(m_savedDepth, m_savedStencilRef);
    ctx->RSSetState(m_savedRaster);
    ctx->OMSetRenderTargets(1, &backBuffer, m_savedDSV);
    Release(m_savedBlend);
    Release(m_savedDepth);
    Release(m_savedRaster);
    Release(m_savedDSV);
    // The post-process pass modified VS, PS, IA and PS SRV/sampler bindings.
    // Rebind the appropriate shaders/input layout before any further 3D drawing.
}
