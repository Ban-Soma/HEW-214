#include "CollisionManager.h"

bool CollisionManager::CheckCollision(const DirectX::XMFLOAT3& APos, const DirectX::XMFLOAT3& AScale, const DirectX::XMFLOAT3& BPos, const DirectX::XMFLOAT3& BScale)
{
	float dx = fabsf(APos.x - BPos.x);
	float dy = fabsf(APos.y - BPos.y);
	float dz = fabsf(APos.z - BPos.z);
	float distanceSq = dx * dx + dy * dy + dz * dz;

	float radius = (AScale.x + BScale.x) * 0.5f; // Assuming uniform scaling for simplicity
	return distanceSq <= radius * radius;
	return false;
}
