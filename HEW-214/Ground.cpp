#include "Ground.h"
#include <DirectXMath.h>
#include "Geometory.h"

Ground::Ground()
	:m_pos{0.0f,0.0f,0.0f}
	,m_scale{1.0f,1.0f,1.0f}
	,m_pTexture(nullptr)
{
	m_pTexture = new Texture();
	HRESULT hr = m_pTexture->Create("Assets/Texture/area.png");
	if (FAILED(hr))
	{
		//“Ç‚Ýž‚ÝŽ¸”s
	}
}

Ground::~Ground()
{
	delete m_pTexture;
}

void Ground::Update()
{

}

void Ground::Draw()
{
	Geometory::SetTexture(m_pTexture ? m_pTexture->GetResource() : nullptr);
	const int FIELD_SIZE = 5;
	const float TILE_SIZE = 5.0f;
	for (int z = 0;z < FIELD_SIZE;z++)
	{
		for (int x = 0;x < FIELD_SIZE;x++)
		{
			float posX = (x - 2) * TILE_SIZE;
			float posZ = (z - 2) * TILE_SIZE;
			//•ÏŠ·s—ñ‚ÌŒvŽZ
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pos[0] + posX, m_pos[1], m_pos[2] + posZ);
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(TILE_SIZE, m_scale[1], TILE_SIZE);
				DirectX::XMMATRIX mat = DirectX::XMMatrixTranspose(S * T);

				//•ÏŠ·s—ñ‚ðGPU‚É“n‚·‚½‚ß‚Ì‘Oˆ—

				DirectX::XMFLOAT4X4 fMat;
				DirectX::XMStoreFloat4x4(&fMat, mat);

			//•`‰æˆ—
			Geometory::SetWorld(fMat);
			Geometory::DrawBox();
		}
	}
	Geometory::SetTexture(nullptr);
}

void Ground::SetPos(float x, float y, float z)
{
	m_pos[0] = x;
	m_pos[1] = y;
	m_pos[2] = z;
}

void Ground::SetScale(float x, float y, float z)
{
	m_scale[0] = x;
	m_scale[1] = y;
	m_scale[2] = z;
}
