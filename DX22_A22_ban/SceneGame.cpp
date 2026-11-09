#include "SceneGame.h"
#include "Defines.h"
#include "Geometory.h"
#include "CameraDebug.h"
#include "Shaderlist.h"

SceneGame::SceneGame()
	:m_pGround(nullptr)
	,m_pGoal(nullptr)
	,m_pCamera(nullptr)
	,m_pModel(nullptr)
{
	m_pGround = new Ground();
	m_pGround->SetPos(0.0f, 0.0f, 0.0f);
	m_pGround->SetScale(3.0f, 0.4f, 3.0f);
	
	m_pGoal = new Goal();
	m_pGoal->SetPos(0.0f, 1.0f, 0.0f);
	m_pGoal->SetScale(0.2f,0.2f,0.2f);

	m_pCamera = new CameraDebug();

	m_pModel = new Model();
	if (!m_pModel->Load("Assets/Model/tree_forest.fbx",0.5f))
	{
		MessageBox(NULL, "モデルの読み込みに失敗しました", "Model Error", MB_OK);
	}
}

SceneGame::~SceneGame()
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pGoal);
	SAFE_DELETE(m_pGround);
}

void SceneGame::Update()
{
	m_pGoal->Update();
	m_pGround->Update();
	m_pCamera->Update();
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

	if (m_pModel)
	{
		//モデルを表示するためのワールド座標を計算
		DirectX::XMFLOAT4X4 fWorld;
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(0.0f, 0.2f, 0.0f);
		DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(0.0f);
		DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(0.0f);
		DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(0.0f);
		DirectX::XMMATRIX R = Rx * Ry * Rz;//回転行列のみを掛け合わせて格納
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
		world = S * R * T;
		world = DirectX::XMMatrixTranspose(world);
		DirectX::XMStoreFloat4x4(&fWorld, world);

		//シェーダーに必要なデータを設定
		ShaderList::SetWorld(fWorld);
		ShaderList::SetView(m_pCamera->GetViewMatrix());
		ShaderList::SetProjection(m_pCamera->GetProjectionMatrix());

		//モデルに使用する各種シェーダーの設定
		m_pModel->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
		m_pModel->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

		//メッシュ別にモデルを表示
		for (int i = 0; i < m_pModel->GetMeshNum(); ++i)
		{
			//モデルのメッシュを取得
			const Model::Mesh* mesh = m_pModel->GetMesh(i);
			//メッシュに割り当てられるマテリアルを取得
			Model::Material material = *m_pModel->GetMaterial(mesh->materialID);

			material.ambient.x = 0.7f;
			material.ambient.y = 0.7f;
			material.ambient.z = 0.7f;
			material.ambient.w = 1.0f;

			//シェーダーにマテリアルを設定
			ShaderList::SetMaterial(material);
			//モデルの描画(内部的に一括で行っていた描画処理を、メッシュごとの描画処理に変更)
			m_pModel->Draw(i);

			//深度テストの有効化
			SetDepthTest(DEPTH_ENABLE);
		}
	}
}