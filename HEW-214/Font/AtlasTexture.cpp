#include "AtlasTexture.h"
#include "TextureAtlas.h"

#include <vector>
#include <cstdint>

namespace
{
    std::vector<uint8_t> ConvertAtlasToRGBA(const TextureAtlas& atlas)
    {
        const std::vector<uint8_t>& source = atlas.GetPixels();
        std::vector<uint8_t> rgba(source.size() * 4);

        for (size_t i = 0; i < source.size(); ++i)
        {
            const uint8_t mask = source[i];
            rgba[i * 4 + 0] = 255;
            rgba[i * 4 + 1] = 255;
            rgba[i * 4 + 2] = 255;
            rgba[i * 4 + 3] = mask;
        }
        return rgba;
    }
}

bool CreateAtlasTexture(ID3D11Device* device, const TextureAtlas& atlas, AtlasTextureDX11& outTexture)
{
    DeleteAtlasTexture(outTexture);

    if (!device || atlas.GetWidth() <= 0 || atlas.GetHeight() <= 0)
        return false;

    const std::vector<uint8_t> rgba = ConvertAtlasToRGBA(atlas);

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = static_cast<UINT>(atlas.GetWidth());
    desc.Height = static_cast<UINT>(atlas.GetHeight());
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = rgba.data();
    init.SysMemPitch = static_cast<UINT>(atlas.GetWidth() * 4);

    HRESULT hr = device->CreateTexture2D(&desc, &init, &outTexture.texture);
    if (FAILED(hr))
    {
        DeleteAtlasTexture(outTexture);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(outTexture.texture, &srvDesc, &outTexture.srv);
    if (FAILED(hr))
    {
        DeleteAtlasTexture(outTexture);
        return false;
    }

    outTexture.width = atlas.GetWidth();
    outTexture.height = atlas.GetHeight();
    return true;
}

bool UpdateAtlasTexture(ID3D11DeviceContext* context, AtlasTextureDX11& texture, const TextureAtlas& atlas)
{
    if (!context || !texture.IsValid())
        return false;

    if (atlas.GetWidth() != texture.width || atlas.GetHeight() != texture.height)
        return false;

    const std::vector<uint8_t> rgba = ConvertAtlasToRGBA(atlas);
    context->UpdateSubresource(
        texture.texture,
        0,
        nullptr,
        rgba.data(),
        static_cast<UINT>(atlas.GetWidth() * 4),
        0);

    return true;
}

void DeleteAtlasTexture(AtlasTextureDX11& texture)
{
    if (texture.srv)
    {
        texture.srv->Release();
        texture.srv = nullptr;
    }
    if (texture.texture)
    {
        texture.texture->Release();
        texture.texture = nullptr;
    }
    texture.width = 0;
    texture.height = 0;
}
