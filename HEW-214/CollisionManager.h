#pragma once
#include <DirectXMath.h>
class CollisionManager
{
public:
	//ƒvƒŒƒCƒ„[‚Æ“G‚Ì“–‚½‚è”»’è
	static bool CheckCollision(
		const DirectX::XMFLOAT3& APos,
		const DirectX::XMFLOAT3& AScale,
		const DirectX::XMFLOAT3& BPos, 
		const DirectX::XMFLOAT3& BScale);
};