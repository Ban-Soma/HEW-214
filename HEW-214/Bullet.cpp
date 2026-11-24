#include "Bullet.h"
#include "Geometory.h"
#include "GameTime.h"

Bullet::Bullet()
	: m_pos{ 0.0f,0.0f,0.0f }
	, m_scale{ 0.2f,0.2f,0.2f }
	, m_direction{ 0.0f,0.0f,0.0f }
	, m_speed(5.0f)
	, m_life(300)
	, m_active(false)
{
}

Bullet::Bullet(DirectX::XMFLOAT3 pos,
	DirectX::XMFLOAT3 direction)
	: Bullet()
{
	Reset(pos, direction);
}

Bullet::~Bullet()
{
	
}

void Bullet::Reset(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 direction)
{
	m_pos = pos;
	m_direction = direction;
	m_scale = { 0.2f,0.2f,0.2f };
	m_speed = 5.0f;
	m_life = 300;
	m_active = true;
}

void Bullet::Update()
{
	if (!m_active)return;
	float dt = GameTime::DeltaTime();
	m_pos.x += m_direction.x * m_speed * dt;
	m_pos.y += m_direction.y * m_speed * dt;
	m_pos.z += m_direction.z * m_speed * dt;

	// ê∂ë∂éûä‘å∏è≠
	m_life-=dt;
	
	if (m_life <= 0)m_active = false;
}

void Bullet::Draw()
{
	if (!m_active)return;
	//ïœä∑çsóÒÇÃåvéZ
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	DirectX::XMMATRIX mat = S * T;

	//ïœä∑çsóÒÇGPUÇ…ìnÇ∑ÇΩÇﬂÇÃëOèàóù
	mat = XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);

	//ï`âÊèàóù
	Geometory::SetWorld(fMat);
	Geometory::DrawBox();
}

void Bullet::SetActive(bool active)
{
	m_active = active;
}
bool Bullet::IsAlive() const
{
	return m_active&&m_life>0;
}
bool Bullet::IsActive()const
{
	return m_active;
}
