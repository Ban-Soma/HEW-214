#pragma once
#include <DirectXMath.h>
class CollisionManager
{
public:
	//ÉvÉåÉCÉÑÅ[Ç∆ìGÇÃìñÇΩÇËîªíË
	static bool CheckCollision(
		const DirectX::XMFLOAT3& APos,
		const DirectX::XMFLOAT3& AScale,
		const DirectX::XMFLOAT3& BPos, 
		const DirectX::XMFLOAT3& BScale);

	static bool CheckSphereCollision(
		const DirectX::XMFLOAT3& aPos, float aRadius, 
		const DirectX::XMFLOAT3& bPos, float bRadius);
	static bool CheckSwapCricle(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		float playerRadius,
		const DirectX::XMFLOAT3& enemyPos,
		float enemyRadius,
		float& hitT
	);

	static void DrawDebugSphere(const DirectX::XMFLOAT3& pos,float radius);

};