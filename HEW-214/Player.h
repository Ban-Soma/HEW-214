#pragma once
#include <DirectXMath.h>

class Enemy;


class Player {
private:
	DirectX::XMFLOAT3 m_pos;//座標
	DirectX::XMFLOAT3  m_scale;//拡大率
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