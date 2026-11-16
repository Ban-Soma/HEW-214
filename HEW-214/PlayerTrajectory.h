#pragma once
#include <DirectXMath.h>
#include <vector>

class Player;

class PlayerTrajectory
{
private:
	DirectX::XMFLOAT3 m_pos;//À•W
	DirectX::XMFLOAT3  m_scale;//Šg‘å—¦

	// ‹OÕ‚ÌÀ•W‚ğ•Û‘¶
	std::vector<DirectX::XMFLOAT3> m_points;

	// ‹OÕ‚ÌÅ‘å•Û‘¶”
	static const int MAX_POINTS = 100;

	// ‹OÕ‚ğ’Ç‰Á‚·‚éŠÔŠu
	float m_addDistance;


public:
	PlayerTrajectory();
	~PlayerTrajectory();

	//XVˆ—
	void Update();
	//•`‰æˆ—
	void Draw();
	//À•W‚Ìİ’è
	void SetPos(float x, float y, float z);
	//Šg‘å—¦‚Ìİ’è
	void SetScale(float x, float y, float z);
	//À•W‚Ìæ“¾
	DirectX::XMFLOAT3 GetPos();
	//‘å‚«‚³‚Ìæ“¾
	DirectX::XMFLOAT3 GetScale();

	// ‹OÕ‚ÌÀ•W‚ğ’Ç‰Á
	void AddPoint(DirectX::XMFLOAT3 pos);
};
