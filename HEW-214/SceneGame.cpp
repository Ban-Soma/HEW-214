#include "SceneGame.h"
#include "Defines.h"
#include "Geometory.h"
#include "CameraDebug.h"
#include "EnemyManager.h"
#include "CombatSystem.h"
#include "time.h"
#include "DirectXMath.h"
#include "CollisionManager.h"


SceneGame::SceneGame()
	:m_pGround(nullptr)
	, m_pGoal(nullptr)
	, m_pCamera(nullptr)
	, m_pModel(nullptr)
	, m_pPlayer(nullptr)
	, m_pEnemyManager(nullptr)
	, m_combatSystem(nullptr)
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

	m_pPlayer = new Player();
	m_pPlayer->LoadFbx("Assets/Model/hero 2.fbx");
	m_pPlayer->SetPos(0.0f, 0.2f, 0.0f);
	m_pPlayer->SetScale(0.2f, 0.2f, 0.2f);
	m_pPlayer->SetCamera(m_pCamera);

	m_pEnemyManager = new EnemyManager();
	m_pEnemyManager->SetEnemyCamera(m_pCamera);
	m_pEnemyManager->InitEnemyManager(m_pPlayer);
	ID3D11Device* device = GetDevice();
	ID3D11DeviceContext* context = GetContext();

	m_debugInfo.Initialize(device, context, "Assets/Fonts/APJapanesefont.ttf");
	m_debugInfo.RegisterPlayer(m_pPlayer);
	m_debugInfo.RegisterEnemyManager(m_pEnemyManager);
	m_debugInfo.RegisterEnergy(m_pPlayer->GetEnergy());
	
	
}

SceneGame::~SceneGame()
{
	SAFE_DELETE(m_pEnemyManager);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pGoal);
	SAFE_DELETE(m_pGround);
	m_debugInfo.Finalize();
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
	
	m_pEnemyManager->UpdateEnemyManager();
	//当たり判定,コンボなどの更新
	m_combatSystem->Update(*m_pPlayer, *m_pEnemyManager);
	m_pEnemyManager->DeadEnemy();
	m_pCamera->SetLook(m_pPlayer->GetPos());

	if (GetAsyncKeyState(VK_F3) & 1)//F3で表示切替
	{
		m_debugInfo.ToggleVisible();
	}
	//デバッグ用数値の更新
	m_debugInfo.Update();
	
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
	//デバッグ用数値の描画
	m_debugInfo.Draw();
	
}