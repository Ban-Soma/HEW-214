#pragma once
class Player;
class EnemyManager;

class CombatSystem {
public:
	void Update(Player& player, EnemyManager& enemyManager);
};