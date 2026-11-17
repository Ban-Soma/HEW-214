#include "Enemy.h"
#include "Player.h"
#include "Geometory.h"
#include "ShaderList.h"
#include "Defines.h"
#include <DirectXMath.h>
#include<cmath>

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
	: m_moveSpeed(0.02f)
	, m_EnemyCount(0)
	, m_EnemyLife(1)
	, m_pEnemy (nullptr)
	, m_pModel(nullptr)
	, m_pCamera(nullptr)
	, m_shotTimer(0)
	, m_shotInterval(60)
{

}

Enemy::~Enemy()
{
	SAFE_DELETE(m_pModel);
}

void Enemy::Update()
{
	FollowingEnemy();

	Shoot();

	/*for (Bullet& bullet : m_bullets)
	{
		bullet.Update();
	}*/
}

void Enemy::Draw()
{
	if (m_pModel == nullptr || m_pCamera == nullptr) return;
	//モデルを表示するためのワールド座標を計算
	DirectX::XMFLOAT4X4 fWorld;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(m_rotation.x);
	DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(m_rotation.y);
	DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(m_rotation.z);
	DirectX::XMMATRIX R = Rx * Ry * Rz;//回転行列のみを掛け合わせて格納
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	world = S * R * T;
	world = DirectX::XMMatrixTranspose(world);
	DirectX::XMStoreFloat4x4(&fWorld, world);

	//シェーダーに必要なデータを設定
	ShaderList::SetWorld(fWorld);
	ShaderList::SetView(m_pCamera->GetViewMatrix());
	ShaderList::SetProjection(m_pCamera->GetProjectionMatrix());

	//モデルに使用する各種シェーダーの設定
	m_pModel->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
	m_pModel->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

	//メッシュ別にモデルを表示
	for (int i = 0; i < m_pModel->GetMeshNum(); ++i)
	{
		//モデルのメッシュを取得
		const Model::Mesh* mesh = m_pModel->GetMesh(i);
		//メッシュに割り当てられるマテリアルを取得
		Model::Material material = *m_pModel->GetMaterial(mesh->materialID);

		material.ambient.x = 0.7f;
		material.ambient.y = 0.7f;
		material.ambient.z = 0.7f;
		material.ambient.w = 1.0f;

		//シェーダーにマテリアルを設定
		ShaderList::SetMaterial(material);
		//モデルの描画(内部的に一括で行っていた描画処理を、メッシュごとの描画処理に変更)
		m_pModel->Draw(i);

		//深度テストの有効化
		SetDepthTest(DEPTH_ENABLE);
	}
	/*for (Bullet& bullet : m_bullets)
	{
		bullet.Draw();
	}*/
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

	//プレイヤーの方に向く計算
	float dx = playerPos.x - m_pos.x;//ｘ方向に差
	float dz = playerPos.z - m_pos.z;//ｚ方向に差
	if (dx * dx + dz * dz)//二乗だから0.01以下なら更新しない
	{
		m_rotation.y = atan2f(dx, dz) ;
	}
		
		

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
	if (g_distance <= 2.5f && g_distance > 0.5f)
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

			// 敵が存在しなければ次の敵へ
			if (m_pEnemy[i] == nullptr) continue;
			//自分自身と等しければcontinue
			if (m_pEnemy[i] == this) continue;
			DirectX::XMFLOAT3 otherPos = m_pEnemy[i]->GetPos();


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

void Enemy::Shoot()
{
	if (g_pPlayer == nullptr) return;

	// プレイヤーの位置を取得
	DirectX::XMFLOAT3 playerPos = g_pPlayer->GetPos();

	// プレイヤーへの方向を計算
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(
		playerPos.x - m_pos.x,
		0.0f,
		playerPos.z - m_pos.z,
		0.0f
	);

	// プレイヤーとの距離を計算
	float distance = DirectX::XMVectorGetX(
		DirectX::XMVector3Length(direction)
	);

	// 射撃する距離
	const float shootDistance = 5.0f;

	// 距離が5.0より遠ければ射撃しない
	if (distance > shootDistance)
	{
		m_shotTimer = 0;
		return;
	}

	// 発射間隔をカウント
	m_shotTimer++;

	// 発射間隔になったら弾を発射
	if (m_shotTimer >= m_shotInterval)
	{
		m_shotTimer = 0;

		// 方向を正規化
		direction = DirectX::XMVector3Normalize(direction);

		DirectX::XMFLOAT3 bulletDirection;
		DirectX::XMStoreFloat3(&bulletDirection, direction);

		//// 弾を生成
		//m_bullets.emplace_back(m_pos, bulletDirection);
	}
}
void Enemy::TakeDamage(int damage)
{
		if (m_EnemyLife <= 0) return; // すでに死んでいる場合は処理しない

		m_EnemyLife -= damage;// ダメージを受ける

		if (m_EnemyLife < 0)// 敵の体力が0未満にならないようにする
			m_EnemyLife = 0;
}


bool Enemy::IsAlive() const
{
	return m_EnemyLife>0;
}

void Enemy::SetEnemyCamera(Camera* camera)
{
	m_pCamera = camera;
}

bool Enemy::LoadEnemyFbx(const char* file)
{
	if (m_pModel == nullptr) m_pModel = new Model();
	return m_pModel->Load(file, 0.5f);
}
