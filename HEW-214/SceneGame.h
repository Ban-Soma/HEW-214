#ifndef __SCENE_GAME_H__
#define __SCENE_GAME_H__

#include "Scene.h"
#include "Ground.h"
#include "Goal.h"
#include "Camera.h"
#include "Model.h"
#include "Player.h"
#include "EnemyManager.h"
#include "CombatSystem.h"
#include "Defines.h"
#include "DebugInfo.h"

class SceneGame : public Scene
{
public:
	SceneGame();
	~SceneGame();
	void Update() final;
	void Draw() final;


private:
	Ground* m_pGround;//地面オブジェクト
	Goal* m_pGoal;//ゴールオブジェクト
	Camera* m_pCamera; //ゲーム中に動かすカメラ
	Model* m_pModel;
	Player* m_pPlayer;
	EnemyManager* m_pEnemyManager;
	CombatSystem* m_combatSystem;

	DebugInfo m_debugInfo; // デバッグ情報表示用
	DebugValues m_debugValues; // デバッグ情報の値
};

#endif // __SCENE_GAME_H__