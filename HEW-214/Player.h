#pragma once
#include <DirectXMath.h>

class Enemy;


class Player {
private:
	DirectX::XMFLOAT3 m_pos;//座標
	DirectX::XMFLOAT3  m_scale;//拡大率
	// ダッシュ用
	bool m_isDashing;     // ダッシュ中か判定
	float m_dashDirX;      // 移動方向X
	float m_dashDirZ;      // 移動方向Z
	float m_dashDistance;  // ダッシュ距離
	float m_dashSpeed;    // 1フレームの移動量
	float m_dashRemaining; // 残りの移動距離
	bool m_prevRT;        // 前フレームRT入力状態

	//矢印
	float m_arrowAngle;  //矢印の回転速度
	float m_stickDirX;   //X軸のスティックを保持
	float m_stickDirZ;	//Y軸のスティックを保持
	bool m_showArrow;//矢印を表示するか
public:
	Player();
	~Player();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//矢印の描画
	void DrawArrow();
	//座標の設定
	void SetPos(float x, float y, float z);
	//拡大率の設定
	void SetScale(float x, float y, float z);
	//座標の取得
	DirectX::XMFLOAT3 GetPos();
	//大きさの取得
	DirectX::XMFLOAT3 GetScale();

	//プレイヤーと敵が重なった時の処理
	int HitPlayer(Enemy** Enemys);
};