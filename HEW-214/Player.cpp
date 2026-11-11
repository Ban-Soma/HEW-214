#include "Player.h"
#include <DirectXMath.h>
#include "Geometory.h"
#include "SceneGame.h"
#include "Input.h"
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")


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
	float MoveSpeed = 0.50f;
	if (IsKeyPress('W')) { m_pos.z += MoveSpeed; }
	if (IsKeyPress('S')) { m_pos.z -= MoveSpeed; }
	if (IsKeyPress('A')) { m_pos.x -= MoveSpeed; }
	if (IsKeyPress('D')) { m_pos.x += MoveSpeed; }

	// Xboxコントローラー
	XINPUT_STATE state = {};

	if (XInputGetState(0, &state) == ERROR_SUCCESS)
	{
		float x = state.Gamepad.sThumbLX / 32768.0f;
		float y = state.Gamepad.sThumbLY / 32768.0f;

		// デッドゾーン
		if (abs(x) < 0.2f) x = 0.0f;
		if (abs(y) < 0.2f) y = 0.0f;

		// 移動
		m_pos.x += x * MoveSpeed;
		m_pos.z += y * MoveSpeed;
	}
}

void Player::Draw()
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

int Player::HitPlayer(Enemy** Enemys, int count)
{
	DirectX::XMFLOAT3 PlayerPos = GetPos();
	DirectX::XMFLOAT3 PlayerScale = GetScale();
	
		for (int i = 0; i < count; i++) {
        // 関数宣言もスペル修正
        int HitPlayer(Enemy** Enemies, int count);
			if (&Enemys[i] == nullptr) {
				continue;
			}

			DirectX::XMFLOAT3 EnemyPos = Enemys[i]->GetEnemyPos();
			DirectX::XMFLOAT3 EnemyScale= Enemys[i]->GetEnemyScale();

			//敵とプレイヤーとの距離計算
			float DistanceX = PlayerPos.x - EnemyPos.x;
			float DistanceZ = PlayerPos.z - EnemyPos.z;

			//幅を半分ずつ足した距離を接触の境界にする処理
			float limitX = (PlayerScale.x + EnemyScale.x) * 0.5;
			float limitZ = (PlayerScale.z + EnemyScale.z) * 0.5;

			if (DistanceX * DistanceX + DistanceZ * DistanceZ < limitX * limitX + limitZ * limitZ) {
				return i;
			}
		}
	
	//最後まで見つからなかったら-1
	return -1;
}

