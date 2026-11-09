#pragma once
class Goal {
private:
	float m_pos[3]; //座標
	float m_scale[3];//すけーる
	float m_time;//アニメーション経過時間

public:
	Goal();
	~Goal();
	//更新処理
	void Update();
	void Draw();
	//座標の設定
	void SetPos(float x, float y, float z);
	//拡大率の設定
	void SetScale(float x, float y, float z);
};