#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Bullet.h"


//クラスの前方宣言　不要なヘッダのインクルードを減らせる。
class Player;



class Enemy {
private:
	Enemy** m_pEnemy;

	int m_shotTimer;       // 発射までの経過フレーム
	int m_shotInterval;    // 発射間隔

	std::vector<Bullet> m_bullets;

	protected:
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_scale;

	float m_moveSpeed; //的の移動スピード
	int m_EnemyCount; //フィールドに生成されてる敵の数
	int m_EnemyLife; //敵のHP


 

public:
	//コンストラクタ
	Enemy();
	//デストラクタ
	~Enemy();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//座標の設定
	void SetEnemyPos(float x, float y, float z);
	//拡大率の設定
	void SetEnemyScale(float x, float y, float z);

	//敵の情報を居ってる配列
	void SetEnemyList(Enemy** enmey, int count);
	//敵の更新
	void FollowingEnemy();
	
	// 弾丸
	void Shoot();

	//敵の体力消費処理
	//virtual void HitDamage(Player* player,int m_EnemyLife) = 0;

	//追従するプレイヤーの座標を取得
	void SetPlayerPos(Player* pPlayer);
	//座標の取得
	DirectX::XMFLOAT3 GetEnemyPos();
	DirectX::XMFLOAT3 GetEnemyScale();
	
	void TakeDamage(int damage);
	
	bool IsAlive() const;
	
};
