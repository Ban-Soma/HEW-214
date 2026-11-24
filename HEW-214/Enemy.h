#pragma once
#include <DirectXMath.h>
#include <GameObject.h>
#include <Camera.h>
#include <Model.h>
#include <vector>
#include "Bullet.h"


//クラスの前方宣言　不要なヘッダのインクルードを減らせる。
class Player;
class BulletPool;


class Enemy : public GameObject  {
public:
	// プレイヤーの状態
	enum State {
		IDLE,   // 待機
		MOVE,   // 移動
		ATACK,  // 攻撃
		DAMAGE, // 被弾
		STUN,   // 行動不能(エネルギー切れなど？)
		DEAD,   // 死亡
		DEADEND,//完全死亡
	};

private:
	State m_State;
	Enemy** m_pEnemy;
	Camera* m_pCamera;
	Model* m_pModel;
	BulletPool* m_pBulletPool;
	bool m_Active;

	float  m_shotTimer;       // 発射までの経過フレーム
	float m_shotInterval;    // 発射間隔

	

	protected:
	float m_moveSpeed; //的の移動スピード
	int m_EnemyCount; //フィールドに生成されてる敵の数
	int m_EnemyLife; //敵のHP

	float m_DeadTimer;


public:
	//コンストラクタ
	Enemy();
	//デストラクタ
	~Enemy();
	//更新処理
	void Update();
	//描画処理
	void Draw();

	//敵の情報を居ってる配列
	void SetEnemyList(Enemy** enmey, int count);
	//敵の更新
	void FollowingEnemy();
	
	// 弾丸
	void Shoot();
	void SetBulletPool(BulletPool* pool);
	//敵の体力消費処理
	//virtual void HitDamage(Player* player,int m_EnemyLife) = 0;

	//追従するプレイヤーの座標を取得
	void SetPlayerPos(Player* pPlayer);
	void TakeDamage(int damage);
	bool IsAlive() const;


	//カメラの処理
	void SetEnemyCamera(Camera* camera);
	bool LoadEnemyFbx(const char* file);
	
	void SetActive(bool active);
	bool IsActive()const;
	void Reset();
	// 状態の設定
	void SetState(State state);
	// 状態の取得
	State GetState();
private:
	// 状態別の更新処理
	void UpdateIdle();
	void UpdateMove();
	void UpdateAtack();
	void UpdateDamage();
	void UpdateStun();
	void UpdateDead();
	void UpdateDeadEnd();
};
