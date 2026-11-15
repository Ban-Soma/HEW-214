#include "Player.h"
#include <DirectXMath.h>
#include "Geometory.h"
#include "SceneGame.h"
#include "Input.h"
#include "EnemyManager.h"
#include "CollisionManager.h"
#include <Xinput.h>
#include <cmath>
#include<algorithm>

#pragma comment(lib, "Xinput.lib")


Player::Player()
	: m_pos{ 0.0f,0.0f,0.0f }
	, m_scale{ 1.0f,1.0f,1.0f }
	// ダッシュ用
	,m_isDashing(false)  // ダッシュ中か判定
	,m_dashDirX (0.0f)      // 移動方向X
	,m_dashDirZ(0.0f)      // 移動方向Z
	,m_dashDistance(3.0f)  // ダッシュ距離
	,m_dashSpeed (0.3f)    // 1フレームの移動量
	,m_dashRemaining(0.0f) // 残りの移動距離
	,m_prevRT (false)// 前フレームRT入力状態
	//矢印表示
	,m_arrowAngle(0.0f)
	,m_stickDirX(0.0f)
	,m_stickDirZ(0.0f)
	,m_showArrow(false)

{

}

Player::~Player()
{

}

void Player::Update()
{
	float MoveSpeed = 0.05f;
	if (IsKeyPress('W')) { m_pos.z += MoveSpeed; }
	if (IsKeyPress('S')) { m_pos.z -= MoveSpeed; }
	if (IsKeyPress('A')) { m_pos.x -= MoveSpeed; }
	if (IsKeyPress('D')) { m_pos.x += MoveSpeed; }

	// Xboxコントローラー
	// コントローラーの入力情報を格納する構造体
	XINPUT_STATE state = {};

	// 現在RTボタンが押されているか
	bool nowRT = false;

	if (XInputGetState(0, &state) == ERROR_SUCCESS)
	{
		// 左スティック
		float x = state.Gamepad.sThumbLX / 32768.0f;
		float z = state.Gamepad.sThumbLY / 32768.0f;

		// デッドゾーン
		float length = sqrtf(x * x + z * z);

		if (length < 0.2f) { x = 0.0f; z = 0.0f; m_showArrow = false;}
		else {
			x /= length; z /= length;// 方向性の正規化
			//スティックの保持
			m_stickDirX = x;
			m_stickDirZ = z;
			m_showArrow = true;

			//矢印の回転速度の計算
			//atan2f 入力方向から角度を計算するための関数
			m_arrowAngle = atan2f(x, z);

		} 
		// RT入力
		nowRT = state.Gamepad.bRightTrigger > 30;

		// RT押した瞬間にダッシュ開始
		if (nowRT && !m_prevRT && !m_isDashing)
		{
			if(length >= 0.2f)
			{
				m_isDashing = true;

				m_dashDirX = x;
				m_dashDirZ = z;

				m_dashRemaining = m_dashDistance;
			}
		}
	}

	m_prevRT = nowRT;

	// ダッシュ中
	if (m_isDashing)
	{
		float move = std::min(m_dashSpeed, m_dashRemaining);

		m_pos.x += m_dashDirX * move;
		m_pos.z += m_dashDirZ * move;

		m_dashRemaining -= move;

		// ダッシュ終了
		if (m_dashRemaining <= 0.0f)
		{
			m_dashRemaining = 0.0f;
			m_isDashing = false;
		}
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
	//矢印の描画
	if (m_showArrow) DrawArrow();
}

void Player::DrawArrow()
{
	//プレイヤーの足元の高さ
	float arrowY = m_pos.y - m_scale.y * 0.5f + 0.03f;

	//矢印の読み込み
	 // 矢印を構成するパーツの描画
	auto DrawArrowPart = [&](float x, float z,
							 float scaleX, float scaleZ,
							 float angle) 
	{
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scaleX, 0.025f, scaleZ);
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(x, 0.0f, z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(m_arrowAngle + angle);
			DirectX::XMMATRIX TPlayer = DirectX::XMMatrixTranslation(m_pos.x, arrowY, m_pos.z);
			DirectX::XMMATRIX mat = S * T * R * TPlayer;
			mat = DirectX::XMMatrixTranspose(mat);
			DirectX::XMFLOAT4X4 fMat;
			DirectX::XMStoreFloat4x4(&fMat, mat);

			Geometory::SetWorld(fMat);
			Geometory::DrawBox();
	};

	// 矢印の棒部分
	DrawArrowPart(0.0f, 0.20f, 0.04f, 0.35f, 0.0f);
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

int Player::HitPlayer(Enemy** Enemys)
{
	DirectX::XMFLOAT3 PlayerPos = GetPos();
	DirectX::XMFLOAT3 PlayerScale = GetScale();
	
		for (int i = 0; i < MAX_ENEMY; i++) {
        // 関数宣言もスペル修正
			if (&Enemys[i] == nullptr) { continue; }

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

