#include "CombatSystem.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "GameTime.h"
CombatSystem::CombatSystem()
{
	m_ShockwaveManager.Init();
}
CombatSystem::~CombatSystem()
{
	m_ShockwaveManager.Uninit();
}
void CombatSystem::Update(Player& player, EnemyManager& enemyManager) {
	m_ShockwaveManager.Update();
	if (!player.IsDashing())return;

	const auto start = player.GetDashPreviousPos();
	const auto end = player.GetPos();

	float nearestT = 2.0f;
	Enemy* hitEnemy = nullptr;
	Enemy* const* enemies = enemyManager.GetEnemies();
	
	for (int i = 0; i < MAX_ENEMY; ++i) {
		Enemy* enemy = enemies[i];

		if (enemy == nullptr || !enemy->IsActive()) continue;
		if (!enemy->IsAlive())continue;

		float hitT = 0.0f;
		if (CollisionManager::CheckSwapCricle(start, end, PLAYER_COLLISION, enemy->GetPos(), ENEMY_COLLISON, hitT))
		{
			if (hitT < nearestT)
			{
				nearestT = hitT;
				hitEnemy = enemy;
			}
		}
	}
	
	if(hitEnemy!=nullptr)
	{	
		DirectX::XMFLOAT3 hitPos = start;
		hitPos.x += (end.x - start.x) * nearestT;
		hitPos.z += (end.z - start.z) * nearestT;
		hitEnemy->TakeDamage(1);

		
		if (!hitEnemy->IsAlive()||!hitEnemy->IsActive())
		{
			player.GetEnergy()->OnEnemyKilled();
			m_ShockwaveManager.Spawn(hitEnemy->GetPos());
			GameTime::Killslow();
		}
		//‚±‚±‚ÌƒIƒ“ƒIƒt‚ÅŠÑ’Ê‚ðØ‚è‘Ö‚¦‚ê‚Ü‚·
		//player.StopDashAt(hitPos);
	}
	player.FinalizeDashStep();
}
void CombatSystem::Draw(Camera* camera)
{
	m_ShockwaveManager.Draw(camera->GetViewMatrix(),camera->GetProjectionMatrix());
}