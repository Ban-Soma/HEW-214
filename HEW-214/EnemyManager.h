#pragma once
#include "Enemy.h"
#include "Defines.h"
#include <vector>

using namespace std;

class Player;

//敵の生成位置を保持する

struct   EnemySpawnPoint
{
	float x;
	float z;
};

class EnemyManager{
private:
	Enemy* m_pEnemy[MAX_ENEMY];

	//スポーン位置の保持
	vector< EnemySpawnPoint> m_spawnPoint;

	//現在使用している位置 -1の場合は未使用
    int m_activeSpawnIndex[MAX_ENEMY];

private:
	//プレイヤーを中心とした範囲判定
	//Inside...(～の中に、中へなど)
	bool IsInsideSpawnArea(float x, float z);

	//敵の生成位置が有効か確認
	//Valid...有効
	bool IsValidSpawnPosition(float x, float z);

	// 敵の生成
	void CreateEnemy(int slot, int spawnIndex);

	//敵同士が重ならないようにする処理



public:
	//コンストラクタ
	EnemyManager();
	//デストラクタ
	~EnemyManager();

	//初期化
	void InitEnemyManager(Player* player);

	//更新
	void UpdateEnemyManager();

	//描画
	void DrawEnemyManager();

	//終了
	void UnInitEnemyManager();

	//敵の数と配置の管理
	//Population...個体数、個体群など文脈によってことなる
	void UpdateEnemyPopulation();

	

};
