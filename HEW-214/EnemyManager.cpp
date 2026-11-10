#include "EnemyManager.h"
#include "Player.h"
#include <cmath>　//計算標準ライブラリ
#include <cstdlib>//乱数など

 extern Player* g_player = new Player;


EnemyManager::EnemyManager()
    :m_pEnemy{nullptr}
    ,m_activeSpawnIndex{-1}
{
 
}

//終了
EnemyManager::~EnemyManager()
{
}

//初期化
void EnemyManager::InitEnemyManager(Player* player)
{
    g_player = player;

    UpdateEnemyPopulation();
}

//更新
void EnemyManager::UpdateEnemyManager()
{
    if (g_player == nullptr) return;

    // 敵の生成・削除・再利用
    UpdateEnemyPopulation();

    // 敵の更新
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (m_pEnemy[i] == nullptr) continue;

        m_pEnemy[i]->Update();
    }
}

//描画
void EnemyManager::DrawEnemyManager()
{
    for (int i = 0; i < MAX_ENEMY; i++) {
        if (m_pEnemy[i] == nullptr) continue;
        m_pEnemy[i]->Draw();
    }
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

//敵の生成位置が有効か
bool EnemyManager::IsValidSpawnPosition(float x, float z)
{
    //敵同士が最低限確保する距離
    const float MidDistance = 0.5f;

    DirectX::XMFLOAT3 PlayerPos = g_player->GetPos();

    //プレイヤーとの距離計算
    //敵の生成予定位置とプレイヤーのX方向の距離
    float DistanceX = x - PlayerPos.x;
    //敵の生成予定位置とプレイヤーのZ方向の距離
    float DistanceZ = z - PlayerPos.z;

    float DistanceSquared =  sqrtf(DistanceX * DistanceX + DistanceZ * DistanceZ);

    //プレイヤーから1.0未満の位置には生成しない
    if (DistanceSquared < 1.0f * 1.0f)    return false;

    // 保存済みスポーン位置との重なり確認
    //保存したずべての生成位置を調べる
    for (const auto& point : m_spawnPoint) {
        
        // 新しい生成位置と保存済み位置のX方向の距離
        float checkX = x - point.x;

        // 新しい生成位置と保存済み位置のZ方向の距離
        float checkZ = z - point.z;

        float CheckDistanceSquared =  sqrtf(checkX * checkX + checkZ * checkZ);

        //保存位置から0.5未満は重なり
        if (CheckDistanceSquared < MidDistance * MidDistance) return false;
    }

    bool HitOverlap = false;
    //活動中の敵との重なり確認
       for (int i = 0; i < MAX_ENEMY; i++)
       {
            if (m_pEnemy[i] == nullptr) continue;

            //活動している敵の座標を取得
            DirectX::XMFLOAT3 OtherPos =  m_pEnemy[i]->GetEnemyPos();

            // 新しい生成位置と敵のX方向の距離
            float CheckX = x - OtherPos.x;

            // 新しい生成位置と敵のZ方向の距離
            float CheckZ = z - OtherPos.z;

            float CheckDistanceSquared = CheckX * CheckX + CheckZ * CheckZ;

            // 敵同士の距離が0.5未満なら生成しない
            if (CheckDistanceSquared <  MidDistance * MidDistance) return false;
       }
}


void EnemyManager::CreateEnemy(int slot, int spawnIndex)
{
    if (slot < 0 || slot >= MAX_ENEMY) return;
    if (spawnIndex < 0 || spawnIndex >= (int) m_spawnPoint.size()) return;
    if (m_pEnemy[slot] != nullptr) return;

    const  EnemySpawnPoint& point = m_spawnPoint[spawnIndex];

    m_pEnemy[slot] = new Enemy();

    m_pEnemy[slot]->SetEnemyPos(point.x, 0.2f, point.z);

    m_pEnemy[slot]->SetEnemyScale(0.2f, 0.2f, 0.2f);

    m_pEnemy[slot]->SetPlayerPos(g_player);

    m_pEnemy[slot]->SetEnemyList(m_pEnemy, MAX_ENEMY);


    m_activeSpawnIndex[slot] = spawnIndex;

}

//敵の数、配置の管理
void EnemyManager::UpdateEnemyPopulation()
{
    if (g_player == nullptr) return;

    //範囲外の敵を消す処理
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (m_pEnemy[i] == nullptr) continue;

        int index = m_activeSpawnIndex[i];

        if (index < 0 || index >= (int)m_spawnPoint.size())
        {
            delete m_pEnemy[i];
            m_pEnemy[i] = nullptr;
            m_activeSpawnIndex[i] = -1;
            continue;
        }

        const EnemySpawnPoint& point = m_spawnPoint[index];

        if (!IsInsideSpawnArea(point.x, point.z))
        {
            delete m_pEnemy[i];
            m_pEnemy[i] = nullptr;

            // スポーン位置の情報は残す
            m_activeSpawnIndex[i] = -1;
        }
    }

    //現在フィールドにいる敵の数を調べる。
    int activeCount = 0;

    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (m_pEnemy[i] != nullptr)
        {
            activeCount++;
        }
    }

    //保存した生成位置から再度敵を生成
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (activeCount >= MAX_ENEMY) break;
        if (m_pEnemy[i] != nullptr) continue;

        for (int j = 0; j < (int)m_spawnPoint.size(); j++)
        {
            // 範囲外なら使わない
            if (!IsInsideSpawnArea(m_spawnPoint[j].x, m_spawnPoint[j].z)) continue;

            // すでに他の敵が使っていないか確認
            bool isActive = false;

            for (int k = 0; k < MAX_ENEMY; k++)
            {
                if (m_pEnemy[k] != nullptr && m_activeSpawnIndex[k] == j)
                {
                    isActive = true;
                    break;
                }
            }

            if (isActive) continue;

            // プレイヤーに近すぎる位置は使わない
            float DistanceX = m_spawnPoint[j].x - g_player->GetPos().x;

            float DistanceZ = m_spawnPoint[j].z - g_player->GetPos().z;

            if (sqrtf(DistanceX * DistanceX + DistanceZ * DistanceZ) < 1.0f)  continue;
            

            CreateEnemy(i, j);
            activeCount++;

            break;
        }
    }

    //フィールド内の敵の数が足りなくなったら補充
   const int MaxRetry = 1000;
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (activeCount >= MAX_ENEMY) break;
        if (m_pEnemy[i] != nullptr) continue;

        bool created = false;

        for (int retry = 0; retry< MaxRetry; retry++)
        {
          DirectX ::  XMFLOAT3 PlayerPos = g_player->GetPos();

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

            activeCount++;
            created = true;

            break;
        }

        // 配置できなかった場合は次の更新で再試行
        if (!created)
        {
            break;
        }
    }
}