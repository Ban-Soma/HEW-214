#include "ShockWaveManager.h"

bool ShockwaveManager::Init()
{
	return m_Renderer.Init();
}

void ShockwaveManager::Uninit()
{
	m_Renderer.Uninit();
}

void ShockwaveManager::Spawn(const DirectX::XMFLOAT3& pos)
{
	for (auto& wave : m_Shockwaves)
	{
		if (wave.IsActive())continue;
		wave.Start(pos);
		return;
	}
}

void ShockwaveManager::Update()
{
	for (auto& wave : m_Shockwaves)
	{
		wave.Update();
	}
}

void ShockwaveManager::Draw(const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	for (auto& wave : m_Shockwaves)
	{
		if (!wave.IsActive())continue;
        const auto pos = wave.GetPos();
        const float radius = wave.GetMaxRadius();
        const float progress = wave.GetProgress();

        // 1. °‚ÉL‚ª‚éÂ”’‚¢Œõ
        m_Renderer.Draw(
            pos,
            radius,
            progress,
            view,
            projection,
            true
        );

        // 2. —‹‚ÌƒŠƒ“ƒO
        m_Renderer.Draw(
            pos,
            radius,
            progress,
            view,
            projection,
            false
        );
		
	}

}
