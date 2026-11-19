#pragma once
#include <DirectXMath.h>
#include <GameObject.h>
#include"PlayerTrajectory.h"
#include "Model.h"
#include "Camera.h"
#include "Energy.h"



class Enemy;


class Player : public GameObject {
public:
	// プレイヤーの状態
	enum State {
		IDLE,   // 待機
		MOVE,   // 通常移動
		DASH,   // ダッシュ
		DAMAGE, // 被弾
		STUN,   // 行動不能(エネルギー切れなど？)
		DEAD,   // 死亡
	};

private:
	State m_state; // 現在の状態

	Model* m_pModel;

	//DirectX::XMFLOAT3 m_pos;//座標
	//DirectX::XMFLOAT3  m_scale;//拡大率
	// ダッシュ用
	float m_dashDirX;      // 移動方向X
	float m_dashDirZ;      // 移動方向Z
	float m_dashDistance;  // ダッシュ距離
	float m_dashSpeed;    // 1フレームの移動量
	float m_dashRemaining; // 残りの移動距離
	bool m_prevRT;        // 前フレームRT入力状態
	bool m_nowRT;         // RTボタンが押されているか

	//矢印
	float m_arrowAngle;  //矢印の回転速度
	float m_stickDirX;   //X軸のスティックを保持
	float m_stickDirZ;	//Y軸のスティックを保持
	bool m_showArrow;//矢印を表示するか


	// 軌跡
	PlayerTrajectory* m_trajectory;

	//カメラ
	Camera* m_pCamera; //カメラ

	Energy* m_pEnergy;
public:
	Player();
	~Player();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//矢印の描画
	void DrawArrow();
	//プレイヤーと敵が重なった時の処理
	int HitPlayer(Enemy** Enemys);
	//カメラの設定
	void SetCamera(Camera* camera);

	bool LoadFbx(const char* file);

	// 状態の設定
	void SetState(State state);
	// 状態の取得
	State GetState();

	Energy* GetEnergy();
private:
	// 状態別の更新処理
	void UpdateIdle();
	void UpdateMove();
	void UpdateDash();
	void UpdateDamage();
	void UpdateStun();
	void UpdateDead();

};