#pragma once
#include "Bullet.h"
#include <array>

class BulletPool
{
public:
	static constexpr size_t MAX_BULLETS = 100;

	void Fire(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 direction);

	void Update();
	void Draw();
	int GetActiveCount()const;

private:
	std::array<Bullet, MAX_BULLETS>m_Bullets;
};