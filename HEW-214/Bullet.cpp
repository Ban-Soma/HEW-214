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

Bullet::~Bullet()
{

}

void Bullet::Update()
{
	m_pos.x += m_direction.x * m_speed;
	m_pos.y += m_direction.y * m_speed;
	m_pos.z += m_direction.z * m_speed;

	m_life--;
}

void Bullet::Draw()
{
	//変換行列の計算
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	DirectX::XMMATRIX mat = S * T;

	//変換行列をGPUに渡すための前処理
	mat = XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);

	//描画処理
	Geometory::SetWorld(fMat);
	Geometory::DrawBox();
}
