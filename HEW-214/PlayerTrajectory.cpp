#include "Geometory.h"
#include <cmath>
#include "PlayerTrajectory.h"


PlayerTrajectory::PlayerTrajectory()
	: m_pos{ 0.0f,0.0f,0.0f }
	, m_scale{ 0.04f,0.025f,1.0f }
	, m_addDistance( 0.1f )
{

}

PlayerTrajectory::~PlayerTrajectory()
{

}

void PlayerTrajectory::Update()
{

}

void PlayerTrajectory::Draw()
{
    // 座標が2つ未満なら描画しない
    if (m_points.size() < 2)
        return;

    for (int i = 0; i < (int)m_points.size() - 1; i++)
    {
       DirectX::XMFLOAT3 p1 = m_points[i];
       DirectX::XMFLOAT3 p2 = m_points[i + 1];

        // 2点間の方向と距離
        float dx = p2.x - p1.x;
        float dz = p2.z - p1.z;

        float length = sqrtf(dx * dx + dz * dz);

        m_scale.z = length;
        // 中間地点
        m_pos.x = (p1.x + p2.x) * 0.5f;
        m_pos.y= (p1.y + p2.y) * 0.5f;
        m_pos.z = (p1.z + p2.z) * 0.5f;

        // Y軸回転
        float angle = atan2f(dx, dz);

        // 軌跡の大きさ
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
            m_scale.x,
            m_scale.y,
            m_scale.z
        );

        // 回転
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

        // 移動
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
            m_pos.x,
            m_pos.y,
            m_pos.z
        );

        DirectX::XMMATRIX mat = S * R * T;

        mat = XMMatrixTranspose(mat);

        DirectX::XMFLOAT4X4 fMat;
        XMStoreFloat4x4(&fMat, mat);

        Geometory::SetWorld(fMat);
        Geometory::DrawBox();
    }
}

void PlayerTrajectory::SetPos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void PlayerTrajectory::SetScale(float x, float y, float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}

DirectX::XMFLOAT3 PlayerTrajectory::GetPos()
{
	return m_pos;
}

DirectX::XMFLOAT3 PlayerTrajectory::GetScale()
{
	return m_scale;
}

void PlayerTrajectory::AddPoint(DirectX::XMFLOAT3 pos)
{
	// 最初の座標
	if (m_points.empty())
	{
		m_points.push_back(pos);
		return;
	}

	// 前回の座標
	DirectX::XMFLOAT3 last = m_points.back();

	// 前回の座標との距離
	float dx = pos.x - last.x;
	float dz = pos.z - last.z;

	float distance = sqrtf(dx * dx + dz * dz);

	// 一定距離移動したら座標を保存
	if (distance >= m_addDistance)
	{
		m_points.push_back(pos);
	}

	// 保存数が上限を超えたら古い座標を削除
	if (m_points.size() > MAX_POINTS)
	{
		m_points.erase(m_points.begin());
	}
}
