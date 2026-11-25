#pragma once
#include "Shockwave.h"
#include "ShockwaveRenderer.h"
#include <array>

class ShockwaveManager
{
public:
    static constexpr int MAX_SHOCKWAVES = 32;

    void Spawn(const DirectX::XMFLOAT3& pos);
    void Update();
    void Draw(const DirectX::XMFLOAT4X4& view,const DirectX::XMFLOAT4X4& projection);
    bool Init();
    void Uninit();

private:
    std::array<Shockwave, MAX_SHOCKWAVES> m_Shockwaves;
    ShockwaveRenderer m_Renderer;
};
