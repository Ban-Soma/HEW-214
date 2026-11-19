#include "Bullet.h"
#include "Geometory.h"

Bullet::Bullet()
	: m_pos{ 0.0f,0.0f,0.0f }
	, m_scale{ 0.2f,0.2f,0.2f }
	, m_direction{ 0.0f,0.0f,0.0f }
	, m_speed(0.1f)
	, m_life(300)
{
}

Bullet::Bullet(DirectX::XMFLOAT3 pos,
	DirectX::XMFLOAT3 direction)
	: m_pos(pos)
	, m_scale{ 0.2f,0.2f,0.2f }
	, m_direction(direction)
	, m_speed(0.01f)
	, m_life(300)
{

}

Bullet::~Bullet()
{

}

void Bullet::Update()
{
	m_pos.x += m_direction.x * m_speed;
	m_pos.y += m_direction.y * m_speed;
	m_pos.z += m_direction.z * m_speed;

	// ¶‘¶ŽžŠÔŒ¸­
	if (m_life > 0)
	{
		m_life--;
	}
}

void Bullet::Draw()
{
	//•ÏŠ·s—ñ‚ÌŒvŽZ
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	DirectX::XMMATRIX mat = S * T;

	//•ÏŠ·s—ñ‚ðGPU‚É“n‚·‚½‚ß‚Ì‘Oˆ—
	mat = XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);

	//•`‰æˆ—
	Geometory::SetWorld(fMat);
	Geometory::DrawBox();
}

bool Bullet::IsAlive() const
{
	return m_life > 0;
}
