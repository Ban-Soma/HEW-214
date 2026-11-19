#pragma once
#include "Texture.h"
class Ground {
private:
	float m_pos[3];//À•W
	float m_scale[3];//Šg‘å—¦
	Texture* m_pTexture;
public:
	Ground();
	~Ground();
	//XVˆ—
	void Update();
	//•`‰æˆ—
	void Draw();
	//À•W‚Ìİ’è
	void SetPos(float x, float y, float z);
	//Šg‘å—¦‚Ìİ’è
	void SetScale(float x, float y, float z);

};