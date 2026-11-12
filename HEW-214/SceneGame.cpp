#include "SceneGame.h"
#include "Defines.h"
#include "Geometory.h"
#include "CameraDebug.h"
#include "EnemyManager.h"
#include "time.h"
#include "DirectXMath.h"

SceneGame::SceneGame()
	:m_pGround(nullptr)
	,m_pGoal(nullptr)
	,m_pCamera(nullptr)
	,m_pModel(nullptr)
	,m_pPlayer(nullptr)
	, m_pEnemyManager(nullptr)
{
	// 乱数の初期化
	srand((unsigned int)time(NULL));

	m_pGround = new Ground();
	m_pGround->SetPos(0.0f, 0.0f, 0.0f);
	m_pGround->SetScale(3.0f, 0.2f, 3.0f);
	
	m_pGoal = new Goal();
	m_pGoal->SetPos(0.0f, 1.0f, 0.0f);
	m_pGoal->SetScale(0.2f,0.2f,0.2f);

	m_pCamera = new CameraDebug();

	m_pModel = new Model();
	if (!m_pModel->Load("Assets/Model/robot.fbx",0.5f, Model::Flip::XFlip))
	{
		MessageBox(NULL, "モデルの読み込みに失敗しました", "Model Error", MB_OK);
	}

	m_pPlayer = new Player();
	m_pPlayer->SetPos(0.0f, 0.2f, 0.0f);
	m_pPlayer->SetScale(0.2f, 0.2f, 0.2f);

	m_pEnemyManager = new EnemyManager();
	m_pEnemyManager->InitEnemyManager(m_pPlayer);
}

SceneGame::~SceneGame()
{
	SAFE_DELETE(m_pEnemyManager);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pGoal);
	SAFE_DELETE(m_pGround);

}

void SceneGame::Update()
{
	m_pGoal->Update();
	m_pGround->Update();
	//プレイヤーを先に動かす
	m_pPlayer->Update();
	//動いた後のプレイヤーの位置をカメラの注視点に設定
	m_pCamera->SetLook(m_pPlayer->GetPos());
	//新しい注視点を使ってカメラの位置を計算
	m_pCamera->Update();
	
	/*r(int i = 0; i < MAX_ENEMY; i++)
	{
		if (m_pEnemy[i] == nullptr) {
			continue;
		}
		//追従するプレイヤーの座標を取得
		m_pEnemy[i]->SetPlayerPos(m_pPlayer);
		m_pEnemy[i]->Update();
	
	}*/

	m_pEnemyManager->UpdateEnemyManager();
	m_pCamera->SetLook(m_pPlayer->GetPos());
	
}

void SceneGame::Draw()
{
#if 0
	Geometory::DrawBox();
	Geometory::DrawCylinder();
	
#endif

	Geometory::SetView(m_pCamera->GetViewMatrix());
	Geometory::SetProjection(m_pCamera->GetProjectionMatrix());

	m_pGround->Draw();
	m_pGoal->Draw();
	m_pPlayer->Draw();	
	m_pEnemyManager->DrawEnemyManager();
	

	/*if (m_pModel)
	{
		m_pModel->Draw();
	}*/
}