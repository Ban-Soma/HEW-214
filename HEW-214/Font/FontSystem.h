#pragma once

#include "Font.h"
#include "TextMesh.h"
#include "TextLayout.h"
#include "AtlasTexture.h"
#include <d3d11.h>
#include <cstdint>
#include <string>
#include <vector>

class FontSystem
{
public:
    FontSystem() = default;
    ~FontSystem();

    FontSystem(const FontSystem&) = delete;
    FontSystem& operator=(const FontSystem&) = delete;

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context,
                    const std::string& fontFile, int pixelSize);

    void Finalize();

    bool IsInitialized() const { return m_initialized; }
    int GetPixelSize() const { return m_font.GetPixelSize(); }

    const Font& GetFont() const { return m_font; }
    Font& GetFont() { return m_font; }

    ID3D11ShaderResourceView* GetAtlasSRV() const { return m_atlasTexture.srv; }

    TextMesh BuildTextMesh(const std::string& utf8Text, float x, float y);
    TextMesh BuildTextMesh(const std::string& utf8Text, const TextLayout& layout,
                           float& outWidth, float& outHeight);

    void DrawMesh(const TextMesh& mesh, float x, float y,
                  float r, float g, float b, float a);

private:
    bool UpdateAtlasIfNeeded();

private:
    Font m_font;
    AtlasTextureDX11 m_atlasTexture;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    uint32_t m_uploadedAtlasRevision = 0;
    bool m_initialized = false;
};
