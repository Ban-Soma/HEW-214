#include "Enemy.h"
#include "Player.h"
#include "Geometory.h"
#include "Defines.h"
#include <DirectXMath.h>

/// <summary>
/// XMVECTORSET() 　x,y,z,wの値をセット
/// XMVector3Length() 3次元ベクトルの長さを計算
/// XMVector3Normalize() 正規化
/// 
/// direction  ：　方向
/// distance   :  距離 
/// </summary>
/// 
/// 
/// 

Player* g_pPlayer = new Player();

Enemy::Enemy()
	: m_pos{0.0f,0.0f,0.0f}
	, m_scale{ 1.0f,1.0f,1.0f }
	, m_moveSpeed(0.02f)
	, m_EnemyCount(0)
	, m_EnemyLife(0)
	, m_pEnemy (nullptr)
{

}

Enemy::~Enemy()
{

}

void Enemy::Update()
{
	FollowingEnemy();
}

void Enemy::Draw()
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

void Enemy::SetEnemyPos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;

}

void Enemy::SetEnemyScale(float x, float y, float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}

void Enemy::SetPlayerPos(Player* pPlayer)
{
	g_pPlayer = pPlayer;
}

//敵の情報をもつ
void Enemy::SetEnemyList(Enemy** enemy, int count)
{
	m_pEnemy = enemy;
	m_EnemyCount = count;
}

//追従処理
void Enemy::FollowingEnemy()
{
	// プレイヤーが設定されていなければ処理を終了
	if (g_pPlayer == nullptr)	return;

	//プレイヤーの位置を取得
	DirectX::XMFLOAT3 playerPos = g_pPlayer->GetPos();

	//プレイヤーへの方向計算
	DirectX::XMVECTOR g_direction = DirectX::XMVectorSet(
		g_pPlayer->GetPos().x - m_pos.x,
		0.0f,
		g_pPlayer->GetPos().z - m_pos.z,
		0.0f
	);

	//プレイヤーとの距離を計算
	float g_distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(g_direction));

	//移動前の位置を候補位置にコピー
	DirectX::XMFLOAT3 nextPos = m_pos;

	//距離が一定以下ならプレイヤーに近づく
	if (g_distance <= 2.0f && g_distance > 0.3)
	{
		//正規化して移動量を計算
		g_direction = DirectX::XMVector3Normalize(g_direction);
		//敵をプレイヤーに向かって移動,//追従方向と速度から候補位置の計算
		nextPos.x += DirectX::XMVectorGetX(g_direction) * m_moveSpeed;
		nextPos.z += DirectX::XMVectorGetZ(g_direction) * m_moveSpeed;
	}

	bool bOverlap = false;
	const float fMinDistance = 0.5f;
	//追従中の敵同士が重ならないようにするための処理 
	if (m_pEnemy != nullptr) {
		for (int i = 0; i < m_EnemyCount; i++) {
			//自分自身と等しければcontinue
			if (m_pEnemy[i] == this) continue;
			DirectX::XMFLOAT3 otherPos = m_pEnemy[i]->GetEnemyPos();


			// 距離計算し、一定距離未満なら分離処理を行う
			float fEnemyDistanceX = nextPos.x - otherPos.x;
			float fEnemyDistanceZ = nextPos.z - otherPos.z;
			float fDistanceSquared = fEnemyDistanceX * fEnemyDistanceX + fEnemyDistanceZ * fEnemyDistanceZ;

			if (fDistanceSquared < fMinDistance * fMinDistance) {
				bOverlap = true;
				break;
			}
		}
	}

	//重なっていないときだけ位置を確定させる
	if (!bOverlap) {
		m_pos = nextPos;
	}
}

DirectX::XMFLOAT3 Enemy::GetEnemyPos()
{
	return m_pos;
}

DirectX::XMFLOAT3 Enemy::GetEnemyScale() {
	return m_scale;
}
