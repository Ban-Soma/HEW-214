#include "CollisionManager.h"
#include "Geometory.h"
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
bool CollisionManager::CheckSphereCollision(const DirectX::XMFLOAT3& aPos,float aRadius,const DirectX::XMFLOAT3& bPos,float bRadius)
{
    float dx = aPos.x - bPos.x;
    float dy = aPos.y - bPos.y;
    float dz = aPos.z - bPos.z;

    float distanceSq = dx * dx + dy * dy + dz * dz;

    float radiusSum = aRadius + bRadius;

    return distanceSq <= radiusSum * radiusSum;
}

void CollisionManager::DrawDebugSphere(const DirectX::XMFLOAT3& pos, float radius)
{
    const int segments = 32;

    for (int i = 0; i < segments; i++)
    {
        float a0 = DirectX::XM_2PI * i / segments;
        float a1 = DirectX::XM_2PI * (i + 1) / segments;

        float c0 = cosf(a0) * radius;
        float s0 = sinf(a0) * radius;
        float c1 = cosf(a1) * radius;
        float s1 = sinf(a1) * radius;

        // XZ•½–ÊF‰¡•ûŒü‚Ì‰~
        Geometory::AddLine(
            { pos.x + c0, pos.y, pos.z + s0 },
            { pos.x + c1, pos.y, pos.z + s1 },
            { 1.0f, 0.0f, 0.0f, 1.0f }
        );

        // XY•½–ÊFc•ûŒü‚Ì‰~
        Geometory::AddLine(
            {pos.x + c0, pos.y + s0, pos.z},
            { pos.x + c1, pos.y + s1, pos.z },
            { 1.0f, 0.0f, 0.0f, 1.0f }
        );

        // YZ•½–ÊF‰œs‚«•ûŒü‚Ì‰~
        Geometory::AddLine(
            { pos.x, pos.y + c0, pos.z + s0 },
            { pos.x, pos.y + c1, pos.z + s1 },
            { 1.0f, 0.0f, 0.0f, 1.0f }
        );
    }
}
