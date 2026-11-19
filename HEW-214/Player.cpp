#include "Player.h"
#include <DirectXMath.h>
#include <ShaderList.h>
#include "Geometory.h"
#include "SceneGame.h"
#include "Input.h"
#include "EnemyManager.h"
#include "CollisionManager.h"
#include <Xinput.h>
#include <cmath>
#include<algorithm>
#include "PlayerTrajectory.h"

#pragma comment(lib, "Xinput.lib")


Player::Player()
    //モデル描画
	:m_pModel(nullptr)
	,m_state(State::IDLE)
	// ダッシュ用
	,m_dashDirX (0.0f)     // 移動方向X
	,m_dashDirZ(0.0f)      // 移動方向Z
	,m_dashDistance(3.0f)  // ダッシュ距離
	,m_dashSpeed (0.3f)    // 1フレームの移動量
	,m_dashRemaining(0.0f) // 残りの移動距離
	,m_prevRT (false)      // 前フレームRT入力状態
	,m_nowRT(false)        // RTを押しているか 
	//矢印表示
	,m_arrowAngle(0.0f)
	,m_stickDirX(0.0f)
	,m_stickDirZ(0.0f)
	,m_showArrow(false)
	//カメラ
	,m_pCamera(nullptr)
{
	m_pEnergy = new Energy;
}

Player::~Player()
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pEnergy);
}

void Player::Update()
{
	const float MoveSpeed = 0.05f;

	//仮の経過時間(60FPS想定)
	//後で実際のdeltaTimeni置き換える
	const float deltaTime = 1.0f / 60.0f;
	m_pEnergy->Update(deltaTime);


	// キーボード移動（現状どおりワールド座標基準）
	if (IsKeyPress('W')) { m_pos.z += MoveSpeed; }
	if (IsKeyPress('S')) { m_pos.z -= MoveSpeed; }
	if (IsKeyPress('A')) { m_pos.x -= MoveSpeed; }
	if (IsKeyPress('D')) { m_pos.x += MoveSpeed; }

	// Xboxコントローラー
	// コントローラーの入力情報を格納する構造体
	XINPUT_STATE state{};

	// 入力状態を初期化
	m_nowRT = false;
	m_showArrow = false;

	if (XInputGetState(0, &state) == ERROR_SUCCESS)
	{
		// 左スティック
		float x = state.Gamepad.sThumbLX / 32768.0f;
		float z = state.Gamepad.sThumbLY / 32768.0f;

		// デッドゾーン
		float length = sqrtf(x * x + z * z);

		float moveX = 0.0f;
		float moveZ = 0.0f;

		if (length >= 0.2f && m_pCamera)
		{
			// 入力方向を単位化
			x /= length;z /= length;

			// カメラからプレイヤーに向かう水平前方向を求める
			DirectX::XMFLOAT3 camPos = m_pCamera->GetPos();
			DirectX::XMVECTOR vForward = DirectX::XMVectorSet(m_pos.x - camPos.x,0.0f,m_pos.z - camPos.z,0.0f);
			vForward = DirectX::XMVector3Normalize(vForward);
			DirectX::XMFLOAT3 forward;
			DirectX::XMStoreFloat3(&forward, vForward);

			// 水平右方向
			DirectX::XMFLOAT3 right{forward.z,0.0f,-forward.x};

			// スティック入力をワールド座標の移動方向に変換
			moveX = right.x * x + forward.x * z;
			moveZ = right.z * x + forward.z * z;

			m_stickDirX = moveX;
			m_stickDirZ = moveZ;
			//矢印の回転
			m_arrowAngle = atan2f(moveX, moveZ);
			//プレイヤの回転
			m_rotation.y = atan2f(moveX, moveZ);
			m_showArrow = true;	
		}

		// スティックがデッドゾーン内でもRT状態を記録する
		m_nowRT = state.Gamepad.bRightTrigger > 30;
	}

	// ステート別の更新処理
	switch (m_state)
	{
	case State::IDLE  : UpdateIdle(); break;
	case State::MOVE  : UpdateMove(); break;
	case State::DASH  : UpdateDash(); break;
	case State::DAMAGE: UpdateDash(); break;
	case State::STUN  : UpdateDash(); break;
	case State::DEAD  : UpdateDash(); break;
	}

	// 前フレームのRT入力状態を取得
	m_prevRT = m_nowRT;
}

void Player::Draw()
{
	if (m_pModel) {
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
	}
	// 軌跡の描画
	m_trajectory.Draw();
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

int Player::HitPlayer(Enemy** Enemys)
{
	DirectX::XMFLOAT3 PlayerPos = GetPos();
	DirectX::XMFLOAT3 PlayerScale = GetScale();
	
		for (int i = 0; i < MAX_ENEMY; i++) {
        // 関数宣言もスペル修正
			if (&Enemys[i] == nullptr) { continue; }

			DirectX::XMFLOAT3 EnemyPos = Enemys[i]->GetPos();
			DirectX::XMFLOAT3 EnemyScale= Enemys[i]->GetScale();

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

void Player::SetCamera(Camera* camera)
{
	m_pCamera = camera;
}

bool Player::LoadFbx(const char* file)
{
	//モデルが生成されてなかったら生成する
	if (m_pModel == nullptr) {
		m_pModel = new Model();
	}
	return m_pModel->Load(file, 0.5f);
}

void Player::SetState(State state)
{
	m_state = state;
}

Player::State Player::GetState()
{
	return m_state;
}

// 待機状態
void Player::UpdateIdle()
{
	// スティック入力がある場合
	if(m_showArrow)
	{
		// RTを押した瞬間
		if (m_nowRT && !m_prevRT)
		{
			m_dashDirX = m_stickDirX;
			m_dashDirZ = m_stickDirZ;
			m_dashRemaining = m_dashDistance;

			SetState(State::DASH);
		}
		else
		{
			SetState(State::MOVE);
		}
	}
}

// 通常移動
void Player::UpdateMove()
{
	
	// スティック入力がなくなったら待機
	if (!m_showArrow)
	{
		SetState(State::IDLE);
		return;
	}

	// RT押した瞬間にダッシュ開始
	if (m_nowRT && !m_prevRT)
	{
		m_dashDirX = m_stickDirX;
		m_dashDirZ = m_stickDirZ;
		m_dashRemaining = m_dashDistance;

		SetState(State::DASH);
	}
}

// ダッシュ
void Player::UpdateDash()
{
	// ダッシュ中の移動量
	float move = std::min(m_dashSpeed, m_dashRemaining);

	m_pos.x += m_dashDirX * move;
	m_pos.z += m_dashDirZ * move;

	m_dashRemaining -= move;

	// ダッシュ終了
	if (m_dashRemaining <= 0.0f)
	{
		m_dashRemaining = 0.0f;
		
		// スティック入力がある場合
		if (m_showArrow)
		{
			SetState(State::MOVE);
		}
		else
		{
			SetState(State::IDLE);
		}
	}
}

// 被弾
void Player::UpdateDamage()
{
}

// 行動不可
void Player::UpdateStun()
{
}

// 死亡
void Player::UpdateDead()
{
}

Energy* Player::GetEnergy()
{
	return m_pEnergy;
}


