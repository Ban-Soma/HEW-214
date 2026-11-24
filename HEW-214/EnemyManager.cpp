#include "EnemyManager.h"
#include "Player.h"
#include <cmath>　//計算標準ライブラリ
#include <cstdlib>//乱数など
#include <chrono>
#include "GameTime.h"
#include "Profiler.h"
#include "CollisionManager.h"
 Player* g_player = nullptr;

 EnemyManager::EnemyManager()
    :m_pEnemy{nullptr}
    ,m_pCamera(nullptr)
    ,m_activeSpawnIndex{}
{
     for (int i = 0;i < MAX_ENEMY;i++)
     {
         m_activeSpawnIndex[i] = -1;
     }
}

//終了
EnemyManager::~EnemyManager()
{
    
    for (int i = 0; i < MAX_ENEMY; i++) {
        SAFE_DELETE(m_pEnemy[i]);
    }
}

//初期化
void EnemyManager::InitEnemyManager(Player* player)
{
    g_player = player;
    //初期状態を設定
    for (int i = 0;i < MAX_ENEMY;i++)
    {
        m_activeSpawnIndex[i] = -1;
        //敵を作成
        m_pEnemy[i] = new Enemy();

        m_pEnemy[i]->SetEnemyCamera(m_pCamera);

        //モデル読み込みは初期化時のみ
        if (!m_pEnemy[i]->LoadEnemyFbx("Assets/Model/teki.fbx"))
        {
            SAFE_DELETE(m_pEnemy[i]);
            continue;
        }
        m_pEnemy[i]->SetScale(0.20f, 0.20f, 0.20f);
        m_pEnemy[i]->SetPlayerPos(g_player);
        m_pEnemy[i]->SetBulletPool(&m_BulletPool);
        m_pEnemy[i]->SetEnemyList(m_pEnemy, MAX_ENEMY);

        // 初期状態では非アクティブ
        m_pEnemy[i]->SetActive(false);
    }
    UpdateEnemyPopulation();
}

//更新
void EnemyManager::UpdateEnemyManager()
{
    PROFILE_SCOPE("EnemyManager.Update");
    if (g_player == nullptr) return;
    // 敵の生成・削除・再利用
    UpdateEnemyPopulation();
    
    // 敵の更新
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Enemy* enemy = m_pEnemy[i];
        if (enemy == nullptr|| !m_pEnemy[i]->IsActive()) continue;
        enemy->Update();
    }
    m_BulletPool.Update();
}

//描画
void EnemyManager::DrawEnemyManager()
{
    for (int i = 0; i < MAX_ENEMY; i++) {
        Enemy* enemy = m_pEnemy[i];
        
        if (enemy == nullptr|| !enemy->IsActive()) continue;
        enemy->Draw();
#ifdef _DEBUG
        CollisionManager::DrawDebugSphere(enemy->GetPos(), ENEMY_COLLISON);
#endif
    }
    m_BulletPool.Draw();
}

//終了
void EnemyManager::UnInitEnemyManager()
{
    for (int i = 0; i < MAX_ENEMY; i++) {
        SAFE_DELETE(m_pEnemy[i]);

        m_activeSpawnIndex[i] = -1;
    }
    m_spawnPoint.clear();
    g_player = nullptr;
}

//プレイヤーの座標を中心に敵を表示させる(10＊10)
bool EnemyManager::IsInsideSpawnArea(float x, float z)
{
    //数字*2バイ  
    const float large = 10.0f;

    DirectX::XMFLOAT3 PlayerPos = g_player->GetPos();
    return fabsf(x - PlayerPos.x) <= large && fabsf(z - PlayerPos.z) <= large;
}



bool EnemyManager::IsValidSpawnPosition(float x, float z)
{
    const float minPlayerDistance = 1.0f;
    const float minEnemyDistance = 0.5f;

    DirectX::XMFLOAT3 playerPos = g_player->GetPos();

    // プレイヤーとの距離
    float dx = x - playerPos.x;
    float dz = z - playerPos.z;

    float distanceSq = dx * dx + dz * dz;

    if (distanceSq < minPlayerDistance * minPlayerDistance)
        return false;

    // 保存済みスポーン位置との距離
    for (const auto& point : m_spawnPoint)
    {
        dx = x - point.x;
        dz = z - point.z;

        distanceSq = dx * dx + dz * dz;

        if (distanceSq < minEnemyDistance * minEnemyDistance)
            return false;
    }

    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (m_pEnemy[i] == nullptr||!m_pEnemy[i]->IsActive())continue;
        DirectX::XMFLOAT3 pos = m_pEnemy[i]->GetPos();
        dx = x - pos.x;
        dz = z - pos.z;
        distanceSq = dx * dx + dz * dz;

        if (distanceSq < minEnemyDistance * minEnemyDistance)
            return false;
    }

    return true;
}

void EnemyManager::CreateEnemy(int slot, int spawnIndex)
{
    if (slot < 0 || slot >= MAX_ENEMY) return;
    if (spawnIndex < 0 || spawnIndex >= (int) m_spawnPoint.size()) return;
    Enemy* enemy = m_pEnemy[slot];
    if (enemy == nullptr)return;
    if (enemy->IsActive())return;

    const  EnemySpawnPoint& point = m_spawnPoint[spawnIndex];

    enemy->Reset();
    enemy->SetPos(point.x, 0.2f, point.z);
    enemy->SetScale(0.20f, 0.20f, 0.20f);
    enemy->SetActive(true);


    m_activeSpawnIndex[slot] = spawnIndex;

}

//敵の数、配置の管理
void EnemyManager::UpdateEnemyPopulation()
{
    PROFILE_SCOPE("EnemyPopulation");
    if (g_player == nullptr) return;
    
    //範囲外の敵を消す処理
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Enemy* enemy = m_pEnemy[i];
        if (m_pEnemy[i] == nullptr||!m_pEnemy[i]->IsActive()) continue;

        int index = m_activeSpawnIndex[i];

        if (index < 0 || index >= (int)m_spawnPoint.size())
        {
            m_pEnemy[i]->SetActive(false);
            m_activeSpawnIndex[i] = -1;
            continue;
        }

        const EnemySpawnPoint& point = m_spawnPoint[index];

        if (!IsInsideSpawnArea(point.x, point.z))
        {
            m_pEnemy[i]->SetActive(false);
            // スポーン位置の情報は残す
            m_activeSpawnIndex[i] = -1;
        }
    }
    
    //現在フィールドにいる敵の数を調べる。
    int activeCount = 0;

    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (m_pEnemy[i] != nullptr&&m_pEnemy[i]->IsActive())
        {
            activeCount++;
        }
    }

    //保存した生成位置から再度敵を生成
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (activeCount >= MAX_ENEMY) break;
        Enemy* enemy = m_pEnemy[i];
        if (enemy == nullptr ||enemy->IsActive()) continue;

        for (int j = 0; j < static_cast<int>(m_spawnPoint.size()); j++)
        {
            const EnemySpawnPoint& point = m_spawnPoint[j];

            // 範囲外なら使わない
            if (!IsInsideSpawnArea(point.x, point.z)) continue;

            // すでに他の敵が使っていないか確認
            bool isActive = false;

            for (int k = 0; k < MAX_ENEMY; k++)
            {
                if (m_pEnemy[k] != nullptr &&m_pEnemy[k]->IsActive()&& m_activeSpawnIndex[k] == j)
                {
                    isActive = true;
                    break;
                }
            }

            if (isActive) continue;
            DirectX::XMFLOAT3 playerPos = g_player->GetPos();

            // プレイヤーに近すぎる位置は使わない
            float dx = point.x - playerPos.x;
            float dz = point.z - playerPos.z;

            if (dx * dx + dz * dz < 1.0f)continue;

            CreateEnemy(i, j);
            if (enemy->IsActive())
            {
                activeCount++;
                break;
            }
        }
    }
    
    //フィールド内の敵の数が足りなくなったら補充
   const int MaxRetry = 30;
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (activeCount >= MAX_ENEMY) break;
        Enemy* enemy = m_pEnemy[i];
        if (enemy == nullptr ||enemy->IsActive())continue;
        bool created = false;

        for (int retry = 0; retry< MaxRetry; retry++)
        {
          DirectX::XMFLOAT3 PlayerPos = g_player->GetPos();

           //20＊20の範囲にランダム配置
            float x = PlayerPos.x +(float)(rand() % 2001) / 100.0f - 10.0f;
            float z = PlayerPos.z +(float)(rand() % 2001) / 100.0f - 10.0f;

            if (!IsValidSpawnPosition(x, z)) continue;

            // 新しいスポーン位置を保存
            EnemySpawnPoint point;
            point.x = x;
            point.z = z;

            m_spawnPoint.push_back(point);

            int spawnIndex = (int)m_spawnPoint.size() - 1;

            CreateEnemy(i, spawnIndex);

            if (enemy->IsActive())
            {
                activeCount++;
                created = true;
                break;
            }
            m_spawnPoint.pop_back();
        }

        // 配置できなかった場合は次の更新で再試行
        if (!created) break;
    }
    
}
//敵配列の取得
Enemy* const* EnemyManager::GetEnemies() const
{
    return m_pEnemy;
}

void EnemyManager::SetEnemyCamera(Camera* camera)
{
    m_pCamera = camera;
}

void EnemyManager::DeadEnemy()
{
    for(int i=0; i<MAX_ENEMY; i++)
    {
		Enemy* enemy = m_pEnemy[i];
		if (enemy == nullptr||!enemy->IsActive()) continue;
		if (enemy->IsAlive()) continue;
        if (enemy->GetState() == Enemy::DEADEND)
        {
            enemy->SetActive(false);
            m_activeSpawnIndex[i] = -1;
            continue;
        }
        if(enemy->GetState()==Enemy::DEAD)continue;
        Energy* t_energy=g_player->GetEnergy();
        t_energy->OnEnemyKilled();
        enemy->SetState(Enemy::DEAD);
	}
}
