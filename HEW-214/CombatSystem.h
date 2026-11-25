#pragma once
#include "ShockWaveManager.h"
#include "Camera.h"
class Player;
class EnemyManager;

class CombatSystem {
public:
	CombatSystem();
	~CombatSystem();
	void Update(Player& player, EnemyManager& enemyManager);
	void Draw(Camera* camera);
private:
	ShockwaveManager m_ShockwaveManager;
};