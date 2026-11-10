#include "Player.h"
#include <DirectXMath.h>
#include "Geometory.h"
#include "SceneGame.h"
#include "Input.h"

Player::Player()
	: m_pos{ 0.0f,0.0f,0.0f }
	, m_scale{ 1.0f,1.0f,1.0f }
{

}

Player::~Player()
{

}

void Player::Update()
{
	if (IsKeyPress('W')) { m_pos.z += 0.04f; }
	if (IsKeyPress('S')) { m_pos.z -= 0.04f; }
	if (IsKeyPress('A')) { m_pos.x -= 0.04f; }
	if (IsKeyPress('D')) { m_pos.x += 0.04f; }
}

void Player::Draw()
{
	//•ÏŠ·s—ñ‚ÌŒvZ
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	DirectX::XMMATRIX mat = S * T;

	//•ÏŠ·s—ñ‚ğGPU‚É“n‚·‚½‚ß‚Ì‘Oˆ—
	mat = XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, mat);


	//•`‰æˆ—
	Geometory::SetWorld(fMat);
	Geometory::DrawBox();
}

void Player::SetPos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void Player::SetScale(float x, float y, float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}

DirectX::XMFLOAT3 Player::GetPos()
{
	return m_pos;
}

DirectX::XMFLOAT3 Player::GetScale()
{
	return m_scale;
}

//int Player::HitPlayer(Enemy** Enemys, int count)
//{
//	DirectX::XMFLOAT3 PlayerPos = GetPos();
//	DirectX::XMFLOAT3 PlayerScale = GetScale();
//	
//		for (int i = 0; i < count; i++) {
//			if (&Enemys[i] == nullptr) {
//				continue;
//			}
//
//			DirectX::XMFLOAT3 EnemyPos = Enemys[i]->GetEnemyPos();
//			DirectX::XMFLOAT3 EnemyScale= Enemys[i]->GetEnemyScale();
//
//			//“G‚ÆƒvƒŒƒCƒ„[‚Æ‚Ì‹——£ŒvZ
//			float DistanceX = PlayerPos.x - EnemyPos.x;
//			float DistanceZ = PlayerPos.z - EnemyPos.z;
//
//			//•‚ğ”¼•ª‚¸‚Â‘«‚µ‚½‹——£‚ğÚG‚Ì‹«ŠE‚É‚·‚éˆ—
//			float limitX = (PlayerScale.x + EnemyScale.x) * 0.5;
//			float limitZ = (PlayerScale.z + EnemyScale.z) * 0.5;
//
//			if (DistanceX * DistanceX + DistanceZ * DistanceZ < limitX * limitX + limitZ * limitZ) {
//				return i;
//			}
//		}
//	
//	//ÅŒã‚Ü‚ÅŒ©‚Â‚©‚ç‚È‚©‚Á‚½‚ç-1
//	return -1;
//}
//
