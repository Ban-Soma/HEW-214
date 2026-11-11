#pragma once
#include <DirectXMath.h>

class Enemy;


class Player {
private:
	DirectX::XMFLOAT3 m_pos;//座標
	DirectX::XMFLOAT3  m_scale;//拡大率
	// ダッシュ用
	bool m_isDashing = false;     // ダッシュ中か判定
	float m_dashDirX = 0.0f;      // 移動方向X
	float m_dashDirZ = 0.0f;      // 移動方向Z
	float m_dashDistance = 3.0f;  // ダッシュ距離
	float m_dashSpeed = 0.3f;    // 1フレームの移動量
	float m_dashRemaining = 0.0f; // 残りの移動距離
	bool m_prevRT = false;        // 前フレームRT入力状態

public:
	Player();
	~Player();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//座標の設定
	void SetPos(float x, float y, float z);
	//拡大率の設定
	void SetScale(float x, float y, float z);
	//座標の取得
	DirectX::XMFLOAT3 GetPos();
	//大きさの取得
	DirectX::XMFLOAT3 GetScale();

	//プレイヤーと敵が重なった時の処理
	int HitPlayer(Enemy** Enemys, int count);
};