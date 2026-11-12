#include "CombatSystem.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyManager.h"

void CombatSystem::Update(Player& player, EnemyManager& enemyManager) {
	Enemy* const* enemies = enemyManager.GetEnemies();
	
	for (int i= 0; i < MAX_ENEMY; ++i) {
		Enemy* enemy = enemies[i];
		
		if (enemy == nullptr) continue;
		if (!enemy->IsAlive())continue;
		bool hit = CollisionManager::CheckCollision(player.GetPos(), player.GetScale(), enemy->GetEnemyPos(), enemy->GetEnemyScale());
			
		if (!hit) continue;
		enemy->TakeDamage(1);
		// プレイヤーと敵が衝突した場合の処理
		// ここにダメージ処理やゲームオーバー処理などを追加することができます。
		// 例: プレイヤーの体力を減らす
		// player.TakeDamage(enemy->GetDamage());
		
	}
}